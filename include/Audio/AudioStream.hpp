#ifndef SHOUTBLAST_AUDIOSTREAM_HPP
#define SHOUTBLAST_AUDIOSTREAM_HPP

#include <thread>
#include <atomic>
#include <memory>
#include <functional>

struct ma_decoder;
struct ma_device;
typedef int ma_result_t;
typedef int ma_seek_origin_t;
typedef unsigned int ma_uint32_t;
typedef signed long long ma_int64_t;

namespace ShoutBlast
{
    typedef void SSL;
    typedef void SSL_CTX;
    class Socket;
    class AudioRingBuffer;
    class AudioBuffer;

    using AudioStreamConnectedEvent = std::function<void(const std::unordered_map<std::string,std::string> &headers)>;
    using AudioStreamDisconnectedEvent = std::function<void()>;
    using AudioStreamMetadataEvent = std::function<void(const std::string &metadata)>;

    class AudioStream
    {
    public:
        AudioStreamConnectedEvent onConnected;
        AudioStreamDisconnectedEvent onDisconnected;
        AudioStreamMetadataEvent onMetadata;
        AudioStream();
        ~AudioStream();
        void Play(const std::string &url);
        void Stop();
        std::string GetCurrentTrack() const;
        float GetVolume() const;
        void SetVolume(float volume);
        void GetPlaybackTime(char *str, size_t size);
        uint64_t GetCurrentPlaybackTime() const;
        uint32_t GetSampleRate() const;
        AudioBuffer *GetOutputBuffer() const;
    private:
        std::thread receiveThread;
        std::atomic<bool> isRunning;
        std::unique_ptr<ma_device> device;
        std::unique_ptr<ma_decoder> decoder;
        std::unique_ptr<AudioRingBuffer> audioRingBuffer;
        std::unique_ptr<AudioBuffer> outputBuffer;
        bool audioInitialized;
        bool isBuffering;
        float volume;
        int metaInterval;
        int bytesUntilMeta;
        std::string currentURL;
        std::string currentTrack;
        std::atomic<uint64_t> currentTime;
        void Reconnect(const std::string &url);
        void Receive(const std::string &url);
        int64_t Read(Socket *socket, SSL *ssl, void *data, size_t size);
        int64_t Write(Socket *socket, SSL *ssl, const void *data, size_t size);
        int FindHeaderEnd(const std::vector<uint8_t> &buffer, int length, std::vector<uint8_t> &collector);
        bool ValidateHeader(const std::vector<uint8_t> &headerBytes, uint32_t &statusCode);
        void ProcessStreamData(Socket *sock, SSL *ssl, const std::vector<uint8_t> &data, int length);
        bool InitializeAudio();
        void UninitializeAudio();
        static ma_result_t OnDecoderRead(ma_decoder* pDecoder, void* pBufferOut, size_t bytesToRead, size_t* pBytesRead);
        static ma_result_t OnDecoderSeek(ma_decoder* pDecoder, ma_int64_t byteOffset, ma_seek_origin_t origin);
        static void OnDeviceData(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32_t frameCount);
    };
}

#endif