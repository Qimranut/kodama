// Kodama regression checks for the FFT, energy normalization, WAV writer,
// binaural response and ray generator fixes.
//
// Self-contained: needs no audio files and no SOFA data. It uses its own
// CHECK macro (not assert), so it still works in Release builds.
//
// Build in Release if you can; the energy tests trace a few hundred
// thousand rays and are slow in Debug.

#include <acoustics/propagator.hpp>
#include <acoustics/attenuation.hpp>
#include <acoustics/ray_generator.hpp>
#include <acoustics/response.hpp>
#include <audio/FFT.hpp>
#include <audio/convolver.hpp>
#include <writer/wav_writer.hpp>   // also included by odr_check.cpp: see that file

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdio>
#include <numbers>
#include <random>
#include <vector>

using namespace kodama;

void kodamaOdrCheck();   // defined in odr_check.cpp

namespace
{
	int g_total = 0;
	int g_failed = 0;

	void report(bool ok, const char* name, const char* detail)
	{
		++g_total;
		if (!ok) ++g_failed;
		std::printf("[%s] %s  %s\n", ok ? "PASS" : "FAIL", name, detail);
	}

	void checkTrue(bool ok, const char* name)
	{
		report(ok, name, "");
	}

	// Passes if |actual - expected| <= relTol * |expected|
	void checkNear(const char* name, double actual, double expected, double relTol)
	{
		char detail[160];
		std::snprintf(detail, sizeof(detail), "actual=%.6g expected=%.6g (tol %.1f%%)",
			actual, expected, relTol * 100.0);
		report(std::fabs(actual - expected) <= relTol * std::fabs(expected), name, detail);
	}

	void checkBelow(const char* name, double value, double limit)
	{
		char detail[160];
		std::snprintf(detail, sizeof(detail), "value=%.3g limit=%.3g", value, limit);
		report(value <= limit, name, detail);
	}

	// ---------------------------------------------------------------- FFT

	void testFFT()
	{
		std::printf("\n-- FFT vs a plain DFT --\n");

		std::mt19937 gen(1);
		std::uniform_real_distribution<float> dist(-1.f, 1.f);

		for (size_t n : { 2u, 4u, 8u, 16u, 64u, 256u, 1024u, 4096u })
		{
			std::vector<std::complex<float>> input(n);
			for (auto& v : input) v = { dist(gen), dist(gen) };

			// Reference: textbook O(n^2) DFT in double precision
			std::vector<std::complex<double>> ref(n);
			double refMax = 0.0;
			for (size_t k = 0; k < n; ++k)
			{
				std::complex<double> sum{};
				for (size_t t = 0; t < n; ++t)
				{
					const double angle = -2.0 * std::numbers::pi * static_cast<double>(k * t % n) / static_cast<double>(n);
					sum += std::complex<double>(input[t]) * std::polar(1.0, angle);
				}
				ref[k] = sum;
				refMax = std::max(refMax, std::abs(sum));
			}

			auto fft = input;
			audio::FFT::transform(fft, false);

			double maxErr = 0.0;
			for (size_t k = 0; k < n; ++k)
				maxErr = std::max(maxErr, std::abs(std::complex<double>(fft[k]) - ref[k]));

			char name[128];
			std::snprintf(name, sizeof(name), "forward FFT matches DFT, n=%zu", n);
			checkBelow(name, maxErr / refMax, 1.e-5);

			auto back = fft;
			audio::FFT::transform(back, true);

			double maxRound = 0.0;
			for (size_t k = 0; k < n; ++k)
				maxRound = std::max(maxRound, static_cast<double>(std::abs(back[k] - input[k])));

			std::snprintf(name, sizeof(name), "inverse(forward(x)) == x, n=%zu", n);
			checkBelow(name, maxRound, 1.e-5);
		}
	}

	void testConvolveFFT()
	{
		std::printf("\n-- FFT convolution vs direct convolution --\n");

		std::mt19937 gen(2);
		std::uniform_real_distribution<float> dist(-1.f, 1.f);
		audio::Convolver convolver;

		for (auto [n, m] : std::vector<std::pair<size_t, size_t>>{ {4, 3}, {200, 50}, {5000, 700} })
		{
			std::vector<float> x(n), h(m);
			for (auto& v : x) v = dist(gen);
			for (auto& v : h) v = dist(gen);

			const auto direct = audio::Convolver::convolve(x, h);
			const auto viaFFT = convolver.convolveFFT(x, h);

			char name[128];
			std::snprintf(name, sizeof(name), "convolveFFT same length, %zux%zu", n, m);
			checkTrue(direct.size() == viaFFT.size(), name);

			double maxDiff = 0.0;
			for (size_t i = 0; i < std::min(direct.size(), viaFFT.size()); ++i)
				maxDiff = std::max(maxDiff, static_cast<double>(std::fabs(direct[i] - viaFFT[i])));

			std::snprintf(name, sizeof(name), "convolveFFT matches direct, %zux%zu", n, m);
			checkBelow(name, maxDiff, 1.e-3);
		}
	}

	// ------------------------------------------------------ capture fraction

	void testCaptureFraction()
	{
		std::printf("\n-- listenerCaptureFraction --\n");

		checkBelow("zero radius gives a tiny fraction",
			acoustics::listenerCaptureFraction(5.f, 0.f), 1.e-5);

		checkNear("source inside the sphere captures every ray",
			acoustics::listenerCaptureFraction(0.4f, 0.5f), 1.0, 1.e-6);

		// Far field: fraction ~ r^2 / (4 d^2)
		checkNear("far field approximates r^2 / (4 d^2)",
			acoustics::listenerCaptureFraction(100.f, 1.f), 1.0 / (4.0 * 100.0 * 100.0), 1.e-3);

		// Exact cone formula (1 - sqrt(1 - (r/d)^2)) / 2 at r/d = 0.5
		checkNear("matches the exact cone formula at r/d = 0.5",
			acoustics::listenerCaptureFraction(2.f, 1.f), 0.5 * (1.0 - std::sqrt(0.75)), 1.e-4);
	}

	// ------------------------------------------------------ energy scaling

	// Summed energy captured from a unit source (strength 1, refDist 1)
	// in free space. The ideal answer is 1 / d^2.
	double freeFieldEnergy(float d, float radius, size_t rayCount, size_t& hits)
	{
		materials::Registry reg;
		scene::Scene scene;
		acoustics::AcousticMedium medium;
		acoustics::Propagator propagator;

		acoustics::SoundSource source{ { 0.f, 0.f, 0.f }, 1.f, 1.f };

		acoustics::Listener listener;
		listener.pos_ = { d, 0.f, 0.f };
		listener.radius_ = radius;

		const auto paths = propagator.propagateMany(source, listener, scene, reg, medium, rayCount, 4);

		hits = paths.size();

		double sum = 0.0;
		for (const auto& p : paths)
			sum += p.energy_;
		return sum;
	}

	void testEnergyScaling()
	{
		std::printf("\n-- free-field energy (ideal = 1/d^2) --\n");

		constexpr size_t rays = 200000;

		double energyAt[4]{};
		const float distances[4]{ 1.f, 2.f, 4.f, 8.f };

		for (int i = 0; i < 4; ++i)
		{
			size_t hits{};
			const double e = freeFieldEnergy(distances[i], 0.5f, rays, hits);
			energyAt[i] = e;

			char name[128];
			std::snprintf(name, sizeof(name), "d=%g: energy follows 1/d^2 (hits=%zu)", distances[i], hits);
			checkTrue(hits >= 100, "  enough hits for a meaningful test");
			checkNear(name, e, 1.0 / (distances[i] * distances[i]), 0.03);
		}

		// An energy that falls as 1/d^4 would give 16 here instead of 4
		checkNear("doubling distance divides energy by 4 (not 16)", energyAt[1] / energyAt[2], 4.0, 0.05);

		// Result must not depend on how many rays are fired...
		size_t h1{}, h2{};
		const double fewRays = freeFieldEnergy(2.f, 0.5f, 50000, h1);
		const double manyRays = freeFieldEnergy(2.f, 0.5f, 200000, h2);
		checkNear("energy independent of ray count (50k vs 200k)", fewRays, manyRays, 0.05);

		// ...or on the size of the listener's capture sphere
		const double smallSphere = freeFieldEnergy(2.f, 0.25f, 200000, h1);
		const double bigSphere = freeFieldEnergy(2.f, 0.5f, 200000, h2);
		checkNear("energy independent of capture radius (0.25 vs 0.5)", smallSphere, bigSphere, 0.05);
	}

	// ------------------------------------------------------- ray generator

	void testRayGenerator()
	{
		std::printf("\n-- RayGenerator --\n");

		const auto dirs = acoustics::RayGenerator::generateDirections(10000);
		checkTrue(dirs.size() == 10000, "generates the requested number of directions");

		double worstLength = 0.0;
		double sx = 0, sy = 0, sz = 0;
		for (const auto& d : dirs)
		{
			worstLength = std::max(worstLength, std::fabs(std::sqrt(static_cast<double>(d.x_ * d.x_ + d.y_ * d.y_ + d.z_ * d.z_)) - 1.0));
			sx += d.x_; sy += d.y_; sz += d.z_;
		}
		checkBelow("every direction has unit length", worstLength, 1.e-4);

		// An even spread over the sphere has a mean direction of ~zero
		const double n = static_cast<double>(dirs.size());
		checkBelow("directions are evenly spread (mean ~ 0)", std::sqrt(sx * sx + sy * sy + sz * sz) / n, 0.01);
	}

	// ---------------------------------------------------- binaural response

	// A made-up HRTF source, so no SOFA file is needed
	struct FakeHRTFSource
	{
		audio::HRTF getHRTF(float azimuth, float elevation) const
		{
			audio::HRTF h;
			h.sampleRate_ = 48000;
			h.left_ = { 0.9f * std::cos(azimuth), 0.4f, -0.2f, 0.1f, 0.05f };
			h.right_ = { 0.7f * std::sin(elevation + 1.f), 0.3f, -0.1f, 0.2f, 0.02f };
			h.leftDelaySec_ = 0.0003f + 0.0001f * std::sin(azimuth);
			h.rightDelaySec_ = 0.0002f;
			return h;
		}
	};

	void testBinaural()
	{
		std::printf("\n-- Response::buildBinaural and fractional delay --\n");

		for (double f : { 0.0, 0.25, 0.5, 0.9 })
		{
			const auto c = acoustics::makeFractionalDelayCoefficients(f);
			char name[128];
			std::snprintf(name, sizeof(name), "fractional-delay taps sum to 1 (fraction %.2f)", f);
			checkNear(name, static_cast<double>(c[0]) + c[1] + c[2] + c[3], 1.0, 1.e-5);
		}

		std::vector<acoustics::Path> paths;
		for (int i = 0; i < 40; ++i)
		{
			acoustics::Path p;
			p.reachedListener_ = true;
			p.energy_ = 0.5f / static_cast<float>(1 + i);
			p.propagationTime_ = 0.004f + 0.00037f * static_cast<float>(i);
			p.arrivalDir_ = math::normalize(math::Vec3{
				std::sin(static_cast<float>(i) * 0.7f),
				std::cos(static_cast<float>(i) * 0.7f),
				0.3f * std::sin(static_cast<float>(i) * 1.3f) });
			paths.push_back(p);
		}

		acoustics::Listener listener;
		listener.pos_ = { 0.f, 0.f, 0.f };
		listener.radius_ = 0.5f;

		const auto ir = acoustics::Response::buildBinaural(paths, 48000, listener, FakeHRTFSource{});

		checkTrue(ir.left_.size() == 910 && ir.right_.size() == 910, "response length is 910 samples");

		bool finite = true;
		double checksum = 0.0;
		for (size_t i = 0; i < ir.left_.size(); ++i)
		{
			finite = finite && std::isfinite(ir.left_[i]) && std::isfinite(ir.right_[i]);
			checksum += ir.left_[i] * static_cast<double>(i % 7 + 1) + ir.right_[i] * static_cast<double>(i % 5 + 1);
		}
		checkTrue(finite, "response contains no NaN or infinity");

		// This number comes from the code before the response.hpp cleanup;
		// the cleanup was not meant to change the output at all.
		checkNear("response unchanged by the cleanup (checksum)", checksum, 20.29931362, 1.e-4);
	}
}

int main()
{
	kodamaOdrCheck();

	testFFT();
	testConvolveFFT();
	testCaptureFraction();
	testRayGenerator();
	testBinaural();
	testEnergyScaling();

	std::printf("\n%d / %d checks passed\n", g_total - g_failed, g_total);
	return g_failed == 0 ? 0 : 1;
}
