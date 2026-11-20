#pragma once

// stds
#include <vector>
#include <cstddef>
#include <complex>
#include <numbers>
#include <cmath>
#include <utility>
#include <assert.h>


namespace kodama::audio
{
	class FFT
	{
	public:

		/**
		 * Performs an in-place discrete Fourier transform,
		 * uses the radix-2 Cooley-Tukey algorithm
		 * 
		 * [param] data
		 * [param] inverse
		 */
		static void transform(
			std::vector<std::complex<float>>& data,
			bool inverse)
		{
			const size_t size = data.size();

			// As the algorithm of choice is radix-2 Cooley-Tukey,
			// input size has to be a power of two
			if (size <= 1)
				return;

			assert((size & (size - 1)) == 0);

			// Bit reversal permutation
			// eg 001 -> 100 or 011 -> 110
			for (size_t i = 1, j = 0; i < size; i++)
			{
				size_t bit = size >> 1;

				for (; j & bit; bit >>= 1)
					j ^= bit;

				j ^= bit;

				if (i < j)
					std::swap(data[i], data[j]);
			}

			// Process increasingly large groups
			for (size_t blockSize = 2; blockSize <= size; blockSize <<= 1)
			{
				// Angle between adjacent roots of unity
				const float angle =
					2.f *
					std::numbers::pi_v<float> /
					static_cast<float>(blockSize) *
					(inverse ? 1.f : -1.f);

				const std::complex<float> root{ std::cos(angle), std::sin(angle) };

				// Process each block
				for (size_t blockStart = 0; blockStart < size; blockStart += blockSize)
				{
					std::complex<float> multiplier{ 1.f, 0.f };

					const size_t halfSize = blockSize / 2;

					for (size_t i = 0; i < halfSize; ++i)
					{
						// Upper half of butterfly
						const auto even = data[blockStart + i];

						// Lower half multiplied by the appropriate root of unity
						const auto odd = data[blockStart + i + halfSize] * multiplier;

						// FFT butterfly
						data[blockStart + i] = even + odd;

						data[blockStart + i + halfSize] = even - odd;

						multiplier *= std::polar(1.f, angle * float(i + 1));
					}
				}
			}

			// The forward trasnform does not normalize
			// The inverse transform divides every result by the transform size
			if (inverse)
				for (auto& value : data)
					value /= static_cast<float>(size);
		}
	};
}
