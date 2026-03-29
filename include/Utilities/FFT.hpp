#ifndef SHOUTBLAST_FFT_HPP
#define SHOUTBLAST_FFT_HPP

#include <cstdlib>
#include <vector>
#include <complex>

namespace ShoutBlast
{
	class FFT
	{
	public:
		static void Perform(std::vector<std::complex<float>> &buffer, size_t n);
	};
}

#endif