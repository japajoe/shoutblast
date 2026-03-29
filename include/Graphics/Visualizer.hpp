#ifndef SHOUTBLAST_VISUALIZER_HPP
#define SHOUTBLAST_VISUALIZER_HPP

#include "FrameBuffer.hpp"
#include "Shader.hpp"
#include <cstdint>
#include <vector>
#include <complex>
#include <string>

namespace ShoutBlast
{
    class AudioBuffer;

    enum class VisualizerMode
    {
        TimeDomain,
        FrequencyDomain
    };

    struct AudioData
    {
        float timeDomain;
        float frequencyDomain;
        float frequencyDomainRaw;
    };

    struct PingPongFBO
    {
		FrameBuffer *src;
		FrameBuffer *dst;
    };

    class Visualizer
    {
    public:
        Visualizer();
        void Generate();
        void OnUpdate(AudioBuffer *audioBuffer);
        void OnGUI();
        bool CompileShader(const std::string &fragmentSource, std::string &error);
    private:
        std::vector<float> timeDomainData;
        std::vector<std::complex<float>> frequencyDomainData;
        std::vector<std::complex<float>> frequencyDomainDataPrevious;
        std::vector<AudioData> audioData;
        FrameBuffer fbo[2];
        PingPongFBO pingpongBuffer;
        Shader shader;
        VisualizerMode mode;
        uint32_t audioTexture;
        uint32_t width;
        uint32_t height;
        uint32_t vao;
        uint32_t uTexture;
        uint32_t uTime;
        uint32_t uSampleRate;
        uint32_t uFrameCount;
        uint32_t uAudio;
        uint32_t uResolution;
        uint32_t uMode;
        uint32_t uBeatPhase;
        uint32_t uBPM;
        void InvalidateFrameBuffer();
        void PerformFFT(AudioBuffer *audioBuffer);
        void UpdateTimeDomainData();
        void UpdateFrequencyDomainData();
        void UpdateAudioTexture();
        void Render();
        float CalculateBpm(const std::vector<std::complex<float>>& currentFrame, const std::vector<std::complex<float>>& previousFrame);
    };
};

#endif