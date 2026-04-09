#include "Visualizer.hpp"
#include "OpenGL.hpp"
#include "Shaders/BasicShader.hpp"
#include "../Audio/AudioSettings.hpp"
#include "../Audio/AudioBuffer.hpp"
#include "../Utilities/FFT.hpp"
#include "../Utilities/Time.hpp"
#include "../../libs/imgui/include/imgui.h"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace ShoutBlast
{
    Visualizer::Visualizer()
    {
        width = 2;
        height = 2;
        vao = 0;
        mode = VisualizerMode::FrequencyDomain;
    }

    void Visualizer::Initialize()
    {
        FrameBufferTextureSpecification colorAttachment = {
            .format = FrameBufferTextureFormat::RGBA8,
            .wrap = TextureWrapMode::ClampToEdge,
            .filter = TextureFilterMode::Linear
        };

        FrameBufferSpecification fboSpec = {
            .width = width,
            .height = height,
            .samples = 1,
            .resizable = true,
            .attachments = {
                colorAttachment
            }
        };

        fbo[0].Generate(fboSpec);
        fbo[1].Generate(fboSpec);

        pingpongBuffer.src = &fbo[0];
        pingpongBuffer.dst = &fbo[1];

        glGenVertexArrays(1, &vao);

        glGenTextures(1, &audioTexture);
        glBindTexture(GL_TEXTURE_2D, audioTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Allocate 2D memory: width x height
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, AudioSettings::GetPeriodSize(), 1, 0, GL_RGB, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        try
        {
            shader.Generate(BasicShader::GetVertexSource(), BasicShader::GetFragmentSource());
            uTime = glGetUniformLocation(shader.GetId(), "Time");
            uSampleRate = glGetUniformLocation(shader.GetId(), "SampleRate");
            uFrameCount = glGetUniformLocation(shader.GetId(), "FrameCount");
            uAudio = glGetUniformLocation(shader.GetId(), "Audio");
            uResolution = glGetUniformLocation(shader.GetId(), "Resolution");
            uMode = glGetUniformLocation(shader.GetId(), "Mode");
            uBeatPhase = glGetUniformLocation(shader.GetId(), "BeatPhase");
            uBPM = glGetUniformLocation(shader.GetId(), "BPM");
            uTexture = glGetUniformLocation(shader.GetId(), "Texture");
        }
        catch(const std::exception &ex)
        {
            std::cout << ex.what() << '\n';
        }

        timeDomainData.resize(AudioSettings::GetPeriodSize());
        frequencyDomainData.resize(AudioSettings::GetPeriodSize());
        frequencyDomainDataPrevious.resize(AudioSettings::GetPeriodSize());
        audioData.resize(AudioSettings::GetPeriodSize() * 2);
    }

    void Visualizer::OnUpdate(AudioBuffer *audioBuffer)
    {
        PerformFFT(audioBuffer);
        UpdateTimeDomainData();
        UpdateFrequencyDomainData();
        UpdateAudioTexture();
        InvalidateFrameBuffer();
        Render();
    }

    void Visualizer::OnGUI()
    {
        ImTextureID texId = pingpongBuffer.src->GetColorAttachment(0);
        ImGui::Begin("Visualizer");
        ImVec2 size = ImGui::GetContentRegionAvail();
        width = static_cast<uint32_t>(size.x);
        height = static_cast<uint32_t>(size.y);
        ImGui::Image(texId, size, ImVec2(0, 0), ImVec2(1, 1));
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            if(mode == VisualizerMode::FrequencyDomain)
                mode = VisualizerMode::TimeDomain;
            else
                mode = VisualizerMode::FrequencyDomain;
        }
        ImGui::End();
    }

    void Visualizer::InvalidateFrameBuffer()
    {
        if(width < 10 || height < 10)
            return;

        if(width != pingpongBuffer.src->GetWidth() || height != pingpongBuffer.src->GetHeight())
        {
            fbo[0].Resize(width, height);
            fbo[1].Resize(width, height);
        }
    }

    void Visualizer::PerformFFT(AudioBuffer *audioBuffer)
    {
        audioBuffer->GetData(timeDomainData);

        if(timeDomainData.size() > 0 && frequencyDomainData.size() > 0)
        {
            size_t size = frequencyDomainData.size() * sizeof(std::complex<float>);
            std::memcpy(frequencyDomainDataPrevious.data(), frequencyDomainData.data(), size);

            for (size_t i = 0; i < timeDomainData.size(); i++) 
            {
                frequencyDomainData[i] = std::complex<float>(timeDomainData[i], 0.0f);
            }
        }

        FFT::Perform(frequencyDomainData, frequencyDomainData.size());


    }

    void Visualizer::UpdateTimeDomainData()
    {
        float historySample = 0.0f; 
        const float smoothness = 0.6f; // 0.0 = raw, 1.0 = flat line

        for (size_t i = 0; i < timeDomainData.size(); i++)
        {
            float raw = timeDomainData[i];

            // The "Smoothing" math: 
            // New Value = (60% of the Old Value) + (40% of the New Value)
            float smoothed = (historySample * smoothness) + (raw * (1.0f - smoothness));

            // Update history so the NEXT iteration knows what happened here
            historySample = smoothed;

            // Interleave into your buffer
            audioData[i].timeDomain = smoothed; 
        }
    }

    void Visualizer::UpdateFrequencyDomainData()
    {
        //const size_t n = frequencyDomainData.size() / 2;
        const size_t n = frequencyDomainData.size();
        const float minDb = -60.0f;
        const float attackSpeed = 0.8f;
        const float falloffSpeed = 0.92f;
        for(size_t i = 0; i < n; i++)
        {
            // 1. Calculate raw magnitude
            float mag = std::abs(frequencyDomainData[i]) / static_cast<float>(n);

            // 2. Convert to Decibels
            // We use 1e-6 to avoid log10(0) which is -infinity
            float db = 20.0f * std::log10(mag + 1e-6f);

            // 3. Normalize to [0, 1] range based on our -60dB floor
            float target = (db - minDb) / (-minDb);
            target = std::max(0.0f, std::min(target, 1.0f));

            // 4. Frequency Tilt (Boost Highs)
            // Raw FFTs are naturally "heavy" in the low end
            float tilt = static_cast<float>(i) / static_cast<float>(n / 2);
            target *= (1.0f + tilt * 2.0f);

            // 5. Temporal Smoothing (The manual 'mix')
            float current = audioData[i].frequencyDomain;

            if (target > current)
            {
                // Attack: Move toward the target
                audioData[i].frequencyDomain = current + (target - current) * attackSpeed;
            }
            else
            {
                // Release: Drift down slowly
                audioData[i].frequencyDomain = current * falloffSpeed;
            }

            audioData[i].frequencyDomainRaw = current;
        }
    }

    void Visualizer::UpdateAudioTexture()
    {
        if (audioData.empty())
            return;

        glBindTexture(GL_TEXTURE_2D, audioTexture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, AudioSettings::GetPeriodSize(), 1, GL_RGB, GL_FLOAT, audioData.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Visualizer::Render()
    {
        static float beatPhase = 0.0f;
        float bpm = CalculateBpm(frequencyDomainData, frequencyDomainDataPrevious);

        if (bpm > 0)
        {
            float beatsPerSecond = bpm / 60.0f;
            beatPhase += Time::GetDeltaTime() * beatsPerSecond;
            if (beatPhase > 1.0f) 
                beatPhase -= 1.0f;
        }

        pingpongBuffer.dst->Bind(); //Frame buffer
        pingpongBuffer.dst->Clear(0, 0, 0, 1);
        
        shader.Use();

        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, audioTexture);

        glActiveTexture(GL_TEXTURE1); 
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer.src->GetColorAttachment(0));

        float resolution[2] = {
            static_cast<float>(pingpongBuffer.dst->GetWidth()),
            static_cast<float>(pingpongBuffer.dst->GetHeight())
        };

        shader.SetIntEx(uAudio, 0);
        shader.SetIntEx(uTexture, 1);
        shader.SetFloatEx(uTime, Time::GetElapsed());
        shader.SetFloatEx(uSampleRate, (float)AudioSettings::GetOutputSampleRate());
        shader.SetFloatEx(uFrameCount, (float)AudioSettings::GetPeriodSize());
        shader.SetFloat2Ex(uResolution, resolution);
        shader.SetIntEx(uMode, static_cast<int>(mode));
        shader.SetFloatEx(uBeatPhase, beatPhase);
        shader.SetFloatEx(uBPM, bpm);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        pingpongBuffer.dst->Unbind();

		auto tmp = pingpongBuffer.src;
		pingpongBuffer.src = pingpongBuffer.dst;
		pingpongBuffer.dst = tmp;

    }

    bool Visualizer::CompileShader(const std::string &fragmentSource, std::string &error)
    {
        static std::string header = R"(#version 330 core

#define MODE_TIME_DOMAIN 0
#define MODE_FREQUENCY_DOMAIN 1

uniform sampler2D Texture;
uniform sampler2D Audio;
uniform float Time;
uniform float SampleRate;
uniform int FrameCount;
uniform vec2 Resolution;
uniform int Mode;
uniform float BeatPhase;
uniform float BPM;

in vec2 TexCoords;
out vec4 FragColor;

)";

        std::string source = header + "\n#line 1\n" + fragmentSource;

        Shader newShader;

        try
        {
            newShader.Generate(BasicShader::GetVertexSource(), source);
            shader.Destroy();
            shader = newShader;
            uTime = glGetUniformLocation(shader.GetId(), "Time");
            uSampleRate = glGetUniformLocation(shader.GetId(), "SampleRate");
            uFrameCount = glGetUniformLocation(shader.GetId(), "FrameCount");
            uAudio = glGetUniformLocation(shader.GetId(), "Audio");
            uResolution = glGetUniformLocation(shader.GetId(), "Resolution");
            uMode = glGetUniformLocation(shader.GetId(), "Mode");
            uBeatPhase = glGetUniformLocation(shader.GetId(), "BeatPhase");
            uBPM = glGetUniformLocation(shader.GetId(), "BPM");
            uTexture = glGetUniformLocation(shader.GetId(), "Texture");
            return true;

        }
        catch(const std::exception &ex)
        {
            error = ex.what();
            return false;
        }
    }

    float Visualizer::CalculateBpm(const std::vector<std::complex<float>>& currentFrame, const std::vector<std::complex<float>>& previousFrame)
    {
        // 1. Persistent State
        static const int historySize = 43;
        static float fluxHistory[historySize] = { 0.0f };
        static int writeIndex = 0;
        static float runningSum = 0.0f;
        
        static long long beatTimestamps[16] = { 0 }; // Increased buffer for better averaging
        static int beatIndex = 0;
        static int beatCount = 0;
        static float smoothedBpm = 0.0f;

        // 2. Calculate Spectral Flux
        float currentFlux = 0.0f;
        size_t lowFreqLimit = std::min(currentFrame.size() / 2, (size_t)40); 

        for (size_t i = 0; i < lowFreqLimit; ++i)
        {
            float diff = std::abs(currentFrame[i]) - std::abs(previousFrame[i]);
            if (diff > 0.0f)
            {
                currentFlux += diff;
            }
        }

        // 3. Update Moving Average
        runningSum -= fluxHistory[writeIndex];
        fluxHistory[writeIndex] = currentFlux;
        runningSum += currentFlux;
        writeIndex = (writeIndex + 1) % historySize;

        float avgFlux = runningSum / (float)historySize;

        // 4. Adaptive Threshold (Stricter detection)
        if (currentFlux > avgFlux * 2.2f) 
        {
            auto now = std::chrono::steady_clock::now();
            long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()).count();

            int lastIdx = (beatIndex + 15) % 16;
            if (beatCount == 0 || (ms - beatTimestamps[lastIdx] > 280)) // Raised debounce
            {
                beatTimestamps[beatIndex] = ms;
                beatIndex = (beatIndex + 1) % 16;
                if (beatCount < 16)
                {
                    beatCount++;
                }
            }
        }

        if (beatCount < 6)
        {
            return 0.0f;
        }

        // 5. Calculate Raw BPM from Median
        float intervals[15];
        int intervalCount = beatCount - 1;
        for (int i = 0; i < intervalCount; i++)
        {
            int curr = (beatIndex - 1 - i + 16) % 16;
            int prev = (beatIndex - 2 - i + 16) % 16;
            intervals[i] = (float)(beatTimestamps[curr] - beatTimestamps[prev]);
        }

        std::sort(intervals, intervals + intervalCount);
        float medianIntervalMs = intervals[intervalCount / 2];
        float rawBpm = 60000.0f / medianIntervalMs;

        // 6. Low-Pass Filter (The Stabilizer)
        // 0.05 is the "Lerp" factor. Lower = more stable but slower to react.
        if (smoothedBpm == 0.0f)
        {
            smoothedBpm = rawBpm;
        }
        else
        {
            smoothedBpm = smoothedBpm + 0.05f * (rawBpm - smoothedBpm);
        }

        return smoothedBpm;
    }
}