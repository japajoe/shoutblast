#include "FFT.hpp"
#include "../Audio/AudioSettings.hpp"

namespace ShoutBlast
{
	static std::vector<float> gHannWindow;
	static void InitializeWindow(size_t n)
	{
		if(gHannWindow.size() > 0)
			return;

		gHannWindow.resize(n);
		for (size_t i = 0; i < n; ++i)
		{
			// Hann window formula: 0.5 * (1 - cos(2 * PI * i / (N - 1)))
			float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(n - 1);
			gHannWindow[i] = 0.5f * (1.0f - std::cos(angle));
		}
	}


	// Assumes n is a power of 2
	void FFT::Perform(std::vector<std::complex<float>> &buffer, size_t n)
	{
		InitializeWindow(n);

		for (size_t i = 0; i < n; ++i)
		{
			buffer[i] *= gHannWindow[i];
		}



		// Bit-reversal permutation
		int j = 0;

		for (int i = 1; i < n; ++i)
		{
			int bit = n >> 1;
			while (j & bit)
			{
				j ^= bit;
				bit >>= 1;
			}
			j ^= bit;
			if (i < j)
				std::swap(buffer[i], buffer[j]);
		}

		// Cooley-Tukey FFT
		for (int len = 2; len <= n; len <<= 1)
		{
			double ang = -2 * M_PI / len;
			std::complex<float> wlen(std::cos(ang), std::sin(ang));
			for (int i = 0; i < n; i += len)
			{
				std::complex<float> w(1);
				for (int j = 0; j < len / 2; ++j)
				{
					std::complex<float> u = buffer[i + j];
					std::complex<float> v = buffer[i + j + len / 2] * w;
					buffer[i + j] = u + v;
					buffer[i + j + len / 2] = u - v;
					w *= wlen;
				}
			}
		}
	}
}