#include "AudioStream.hpp"
#include "AudioSettings.hpp"
#include "AudioRingBuffer.hpp"
#include "AudioBuffer.hpp"
#include "../Utilities/HttpClient.hpp"
#include "../Utilities/OpenSSL.hpp"
#include "../Utilities/String.hpp"
#include "../../libs/miniaudio/include/miniaudio.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <unordered_map>

namespace ShoutBlast
{
    static constexpr int BUFFER_CAPACITY = 1024 * 128;
    static constexpr int BUFFER_THRESHOLD = 1024 * 64;

    AudioStream::AudioStream()
    {
        isRunning.store(false);
        audioRingBuffer = std::make_unique<AudioRingBuffer>(BUFFER_CAPACITY);
        outputBuffer = std::make_unique<AudioBuffer>();
        decoder = std::make_unique<ma_decoder>();
        device = std::make_unique<ma_device>();
        volume = 1.0f;
        audioInitialized = false;
        isBuffering = true;
        metaInterval = 0;
        bytesUntilMeta = 0;
        currentTime.store(0);

        std::memset(decoder.get(), 0, sizeof(ma_decoder));
        std::memset(device.get(), 0, sizeof(ma_device));

        SSL_library_load();
    }

    AudioStream::~AudioStream()
    {
        Stop();
    }

    void AudioStream::Play(const std::string &url)
    {
        Stop();

        isRunning.store(true);
        receiveThread = std::thread(&AudioStream::Receive, this, url);
    }

    void AudioStream::Stop()
    {
        isRunning.store(false);
        if (receiveThread.joinable())
        {
            receiveThread.join();
        }
    }

    void AudioStream::Reconnect(const std::string &url)
    {
        Play(url);
    }

    void AudioStream::Receive(const std::string &url)
    {
        currentURL = url;
        
    START:
        std::string ip;
        std::string host;
        std::string path;
        uint16_t port;
        const bool useSsl = (currentURL.substr(0, 8) == "https://");

        Uri uri;

        if(!Uri::TryParse(currentURL, uri))
            return;

        ip = uri.GetIP();
        host = uri.GetHost();
        path = uri.GetPathAndQueuery();
        port = uri.GetPort();

        Socket sock = Socket();

        if(!sock.Connect(ip, port, 5000))
        {
            sock.Close();
            return;
        }

        SSL_CTX *ctx = nullptr;
        SSL *ssl = nullptr;

        if (useSsl)
        {
            ctx = SSL_CTX_new(TLS_client_method());
            ssl = SSL_new(ctx);
            SSL_set_fd(ssl, sock.GetDescriptor());
            if (SSL_connect(ssl) <= 0)
            {
                SSL_free(ssl);
                SSL_CTX_free(ctx);
                sock.Close();
                return;
            }
        }

        std::string request = "GET " + path + " HTTP/1.1\r\n" +
                            "Host: " + host + "\r\n" +
                            "User-Agent: MiniAudioEx/1.0\r\n" +
                            "Icy-MetaData: 1\r\n" +
                            "Connection: close\r\n\r\n";

        const char *requestPtr = request.c_str();
        size_t totalSent = 0;
        size_t requestLen = request.length();

        while (totalSent < requestLen && isRunning)
        {
            int sent = 0;
            sent = Write(&sock, ssl, requestPtr + totalSent, (int)(requestLen - totalSent));

            if (sent <= 0)
            {
                break;
            }
            totalSent += sent;
        }

        std::vector<uint8_t> buffer(4096);
        std::vector<uint8_t> headerCollector;
        bool headerSkipped = false;
        uint32_t statusCode = 0;
        bool reconnect = false;

        while (isRunning)
        {
            int bytesRead = 0;
            bytesRead = Read(&sock, ssl, buffer.data(), buffer.size());

            if (bytesRead <= 0)
            {
                reconnect = true;
                break;
            }

            if (!headerSkipped)
            {
                int payloadOffset = FindHeaderEnd(buffer, bytesRead, headerCollector);

                if (payloadOffset != -1)
                {
                    if (!ValidateHeader(headerCollector, statusCode))
                    {
                        std::cout << "Failed to validate header\n";
                        isRunning.store(false);
                        break;
                    }

                    headerSkipped = true;
                    int remainingBytes = bytesRead - payloadOffset;
                    if (remainingBytes > 0)
                    {
                        std::vector<uint8_t> remainingData(remainingBytes);
                        std::memcpy(remainingData.data(), &buffer[payloadOffset], remainingBytes);
                        ProcessStreamData(&sock, ssl, remainingData, remainingBytes);
                    }
                }
                continue;
            }

            ProcessStreamData(&sock, ssl, buffer, bytesRead);

            if (!audioInitialized && audioRingBuffer->GetCount() >= BUFFER_THRESHOLD)
            {
                if (!InitializeAudio())
                {
                    break;
                }
            }
            else if (isBuffering && audioRingBuffer->GetCount() >= BUFFER_THRESHOLD)
            {
                isBuffering = false;
            }
        }

        if (useSsl)
        {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
        }

        sock.Close();

        UninitializeAudio();

        if(reconnect)
        {
            std::cout << "Reconnecting...\n";
            goto START;
        }

        if(onDisconnected)
            onDisconnected();

        // if(statusCode == 302 && redirectLocation.size() > 0)
        // {
        //     goto START_RECEIVE;
        // }
    }

    int64_t AudioStream::Read(Socket *socket, SSL *ssl, void *data, size_t size)
    {
        if (ssl)
            return SSL_read(ssl, data, size);
        else
            return socket->Read(data, size);
    }

    int64_t AudioStream::Write(Socket *socket, SSL *ssl, const void *data, size_t size)
    {
        if (ssl)
            return SSL_write(ssl, data, size);
        else
            return socket->Write(data, size);
    }

    int AudioStream::FindHeaderEnd(const std::vector<uint8_t> &buffer, int length, std::vector<uint8_t> &collector)
    {
        for (int i = 0; i < length; i++)
        {
            collector.push_back(buffer[i]);

            if (collector.size() >= 4)
            {
                size_t last = collector.size();
                if (collector[last - 4] == 0x0D && collector[last - 3] == 0x0A &&
                    collector[last - 2] == 0x0D && collector[last - 1] == 0x0A)
                {
                    return i + 1;
                }
            }
        }
        return -1;
    }

    bool AudioStream::ValidateHeader(const std::vector<uint8_t> &headerBytes, uint32_t &statusCode)
    {
        std::string headerText(headerBytes.begin(), headerBytes.end());

        std::vector<std::string> lines;
        std::string line;
        std::stringstream ss(headerText);

        while (std::getline(ss, line))
        {
            if (!line.empty() && line.back() == '\r')
                line.pop_back(); // remove CR

            lines.push_back(line);
        }

        if (lines.size() == 0)
        {
            return false;
        }

        lines[0] = String::Trim(lines[0]);

        if(String::Contains(lines[0], "HTTP/", true))
        {
            auto parts = String::Split(lines[0], " ");
            if(parts.size() > 1)
            {
                if(!String::TryParseUInt32(parts[1], statusCode))
                    return false;
            }
        }

        if(statusCode != 200 && statusCode != 302)
            return false;

        bool isAudio = false;

        std::unordered_map<std::string,std::string> responseHeaders;

        for (const auto &line : lines)
        {
            int separatorIndex = String::IndexOf(line, ":");
            if (separatorIndex != -1)
            {
                std::string key = String::SubString(line, 0, separatorIndex);
                std::string value = String::SubString(line, separatorIndex + 1);

                key = String::Trim(key);
                value = String::Trim(value);

                responseHeaders[key] = value;

                if (String::ToLower(key) == "content-type")
                {
                    if (String::Contains(value, "audio/", true) || String::Contains(value, "application/octet-stream", true))
                    {
                        isAudio = true;
                    }
                }

                if (String::ToLower(key) == "icy-metaint")
                {
                    int32_t interval = 0;
                    if (String::TryParseInt32(value, interval))
                    {
                        metaInterval = interval;
                        bytesUntilMeta = metaInterval;
                    }
                }

                if(statusCode == 302)
                {
                    if (String::ToLower(key) == "location")
                        currentURL = value;
                }
            }
        }

        if(onConnected)
            onConnected(responseHeaders);

        return isAudio && (statusCode == 200);
    }

    void AudioStream::ProcessStreamData(Socket *sock, SSL *ssl, const std::vector<uint8_t> &data, int length)
    {
        if (metaInterval <= 0)
        {
            audioRingBuffer->Write(data.data(), 0, length);
            return;
        }

        int offset = 0;
        while (offset < length)
        {
            if (bytesUntilMeta > 0)
            {
                int toWrite = std::min(bytesUntilMeta, length - offset);
                audioRingBuffer->Write(data.data(), offset, toWrite);
                bytesUntilMeta -= toWrite;
                offset += toWrite;
            }
            else
            {
                // Read meta length byte
                int lengthByte = data[offset];
                offset++;

                if (lengthByte > 0)
                {
                    int metaLength = lengthByte * 16;
                    std::vector<uint8_t> metaBuffer(metaLength);
                    int metaRead = 0;

                    // Metadata might span across network packets
                    while (metaRead < metaLength)
                    {
                        if (offset < length)
                        {
                            metaBuffer[metaRead++] = data[offset++];
                        }
                        else
                        {
                            int r = Read(sock, ssl, &metaBuffer[metaRead], metaLength - metaRead);
                            if (r <= 0)
                            {
                                break;
                            }
                            metaRead += r;
                        }
                    }

                    std::string metaString(metaBuffer.begin(), metaBuffer.end());

                    if(onMetadata)
                    {
                        std::string key = "StreamTitle='";
                        size_t start = metaString.find(key);
                        
                        if (start != std::string::npos) 
                        {
                            start += key.length();
                            size_t end = metaString.find("'", start);
                            
                            if (end != std::string::npos) 
                            {
                                currentTrack = metaString.substr(start, end - start);
                            }
                        }


                        onMetadata(metaString);
                    }
                }

                bytesUntilMeta = metaInterval;
            }
        }
    }

    bool AudioStream::InitializeAudio()
    {
        ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, AudioSettings::GetOutputChannels(), AudioSettings::GetOutputSampleRate());

        ma_decoder_read_proc readProc = reinterpret_cast<ma_decoder_read_proc>(OnDecoderRead);
        ma_decoder_seek_proc seekProc = reinterpret_cast<ma_decoder_seek_proc>(OnDecoderSeek);

        ma_result result = ma_decoder_init(readProc, seekProc, this, &decoderConfig, decoder.get());

        if (result != MA_SUCCESS)
        {
            std::cout << "Failed to create decoder: " << result << '\n';
            return false;
        }

        ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format = decoder->outputFormat;
        deviceConfig.playback.channels = decoder->outputChannels;
        deviceConfig.sampleRate = decoder->outputSampleRate;
        deviceConfig.dataCallback = OnDeviceData;
        deviceConfig.pUserData = this;
        deviceConfig.periodSizeInFrames = AudioSettings::GetPeriodSize();

        result = ma_device_init(nullptr, &deviceConfig, device.get());

        if (result != MA_SUCCESS)
        {
            std::cout << "Failed to create device\n";
            ma_decoder_uninit(decoder.get());
            return false;
        }

        audioInitialized = true;
        currentTime.store(0);
        ma_device_start(device.get());
        return true;
    }

    void AudioStream::UninitializeAudio()
    {
        if (audioInitialized)
        {
            ma_device_stop(device.get());
            ma_device_uninit(device.get());
            ma_decoder_uninit(decoder.get());
            audioInitialized = false;
            currentTime.store(0);
            audioRingBuffer->Reset();
        }
    }

    ma_result_t AudioStream::OnDecoderRead(ma_decoder* pDecoder, void* pBufferOut, size_t bytesToRead, size_t* pBytesRead)
    {
        AudioStream *stream = reinterpret_cast<AudioStream*>(pDecoder->pUserData);

        if (stream->isBuffering && stream->audioInitialized)
        {
            pBytesRead = 0;
            return MA_SUCCESS;
        }

        int actualRead = stream->audioRingBuffer->Read(pBufferOut, bytesToRead);
        
        *pBytesRead = actualRead;

        if (actualRead > 0)
        {
            return MA_SUCCESS;
        }
        
        if (stream->isRunning.load())
        {
            if (stream->audioInitialized)
            {
                stream->isBuffering = true;
            }
            return MA_SUCCESS;
        }

        return MA_AT_END;
    }

    ma_result_t AudioStream::OnDecoderSeek(ma_decoder* pDecoder, ma_int64_t byteOffset, ma_seek_origin_t origin)
    {
        return MA_SUCCESS;
    }

    void AudioStream::OnDeviceData(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32_t frameCount)
    {
        if (pDevice == nullptr)
            return;

        AudioStream *stream = reinterpret_cast<AudioStream*>(pDevice->pUserData);

        if (ma_decoder_read_pcm_frames(stream->decoder.get(), pOutput, frameCount, nullptr) == MA_SUCCESS)
        {
            const uint32_t channels = stream->decoder->outputChannels;
            const uint32_t totalSamples = frameCount * channels;
            const float volume = stream->volume;

            stream->currentTime.fetch_add(frameCount, std::memory_order_relaxed);

            //std::cout << "Frame count: " << frameCount << '\n';
            
            float *data = reinterpret_cast<float*>(pOutput);
            
            if (volume == 1.0f)
            {
                stream->outputBuffer->SetData(data, frameCount, channels);
                return;
            }
            
            
            for (int i = 0; i < totalSamples; i++)
            {
                data[i] *= volume;
            }

            stream->outputBuffer->SetData(data, frameCount, channels);
        }
    }

    std::string AudioStream::GetCurrentTrack() const
    {
        return currentTrack;
    }

    float AudioStream::GetVolume() const
    {
        return volume;
    }

    void AudioStream::SetVolume(float volume)
    {
        if(volume < 0.0f)
            volume = 0.0f;
        if(volume > 1.0f)
            volume = 1.0f;
        this->volume = volume;
    }

    void AudioStream::GetPlaybackTime(char *str, size_t size)
    {
        if(!str)
            return;

        if(size < 9)
            return;

        uint64_t time = GetCurrentPlaybackTime();
        
        if(time == 0)
        {        
            str[0] = '0';
            str[1] = '0';
            str[2] = ':';
            str[3] = '0';
            str[4] = '0';
            str[5] = ':';
            str[6] = '0';
            str[7] = '0';
            str[8] = '\0';
            return;
        }

        uint64_t totalSeconds = time / GetSampleRate();        
        uint32_t hours = totalSeconds / 3600;
        uint32_t minutes = (totalSeconds % 3600) / 60;
        uint32_t seconds = totalSeconds % 60;

        if(hours >= 0 && hours < 100)
            snprintf(str, size, "%02u:%02u:%02u", hours, minutes, seconds);
        else if(hours >= 100 && hours < 1000)
            snprintf(str, size, "%03u:%02u:%02u", hours, minutes, seconds);
        else
            snprintf(str, size, "%04u:%02u:%02u", hours, minutes, seconds);
    }

    uint64_t AudioStream::GetCurrentPlaybackTime() const
    {
        return currentTime.load(std::memory_order_relaxed);
    }

    uint32_t AudioStream::GetSampleRate() const
    {
        if(!decoder)
            return 44100;
        return decoder->outputSampleRate;
    }

    AudioBuffer *AudioStream::GetOutputBuffer() const
    {
        return outputBuffer.get();
    }
}