#include <geometry/plane.hpp>
#include <geometry/triangle.hpp>
#include <scene/scene.hpp>
#include <material/material_reg.hpp>
#include <acoustics/ray.hpp>
#include <acoustics/propagator.hpp>
#include <acoustics/response.hpp>
#include <audio/convolver.hpp>
#include <audio/buffer.hpp>	
#include <audio/spatializer.hpp>
#include <audio/HRTF.hpp>
#include <audio/Sofa_HRTF_provider.hpp>
#include <acoustics/response.hpp>
#include <writer/wav_writer.hpp>

// stds
#include <filesystem>

#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>
#include <utility>


const std::filesystem::path assetDir = KODAMA_ASSET_DIR;
const std::filesystem::path outputDir = KODAMA_OUTPUT_DIR;


int main()
{
	//const std::vector<float> input{
	//		1.0f,
	//		0.5f,
	//		0.0f,
	//		-0.5f
	//};

	//const std::vector<float> impulseResponse{
	//	1.0f,
	//	0.0f,
	//	0.5f
	//};

	//kodama::audio::Convolver convolver;

	//// Reference implementation.
	//const auto direct =
	//	convolver.convolve(
	//		input,
	//		impulseResponse);

	//// FFT implementation.
	//const auto fft =
	//	convolver.convolveFFT(
	//		input,
	//		impulseResponse);

	//if (direct.size() != fft.size())
	//{
	//	std::cout << "Size mismatch!\n";
	//	return 1;
	//}

	//// Compare every output sample.
	//float maximumDifference = 0.0f;

	//for (std::size_t i = 0;
	//	i < direct.size();
	//	++i)
	//{
	//	const float difference =
	//		std::fabs(
	//			direct[i] - fft[i]);

	//	maximumDifference =
	//		std::fmax(
	//			maximumDifference,
	//			difference);
	//}

	//std::cout << "Maximum difference: "
	//	<< maximumDifference
	//	<< '\n';

	//////////////////////////////////////////

	//kodama::acoustics::Listener listener{
	//{ 0.0f, 0.0f, 0.0f }
	//};

	//kodama::audio::Spatializer spatializer;

	//// Sound arriving from directly in front.
	//// Sound travels toward the listener along -Y.
	//kodama::acoustics::Path frontPath;
	//frontPath.arrivalDir_ = {
	//	0.0f, -1.0f, 0.0f
	//};

	//// Sound arriving from the listener's right.
	//// Sound travels toward the listener along -X.
	//kodama::acoustics::Path rightPath;
	//rightPath.arrivalDir_ = {
	//	-1.0f, 0.0f, 0.0f
	//};

	//// Sound arriving from the listener's left.
	//// Sound travels toward the listener along +X.
	//kodama::acoustics::Path leftPath;
	//leftPath.arrivalDir_ = {
	//	1.0f, 0.0f, 0.0f
	//};

	//const auto front =
	//	spatializer.computeGains(
	//		frontPath,
	//		listener);

	//const auto right =
	//	spatializer.computeGains(
	//		rightPath,
	//		listener);

	//const auto left =
	//	spatializer.computeGains(
	//		leftPath,
	//		listener);

	//std::cout
	//	<< "Front: "
	//	<< front.left_ << ", "
	//	<< front.right_ << '\n';

	//std::cout
	//	<< "Right: "
	//	<< right.left_ << ", "
	//	<< right.right_ << '\n';

	//std::cout
	//	<< "Left: "
	//	<< left.left_ << ", "
	//	<< left.right_ << '\n';

	//////////////////////////////////////////

	/*kodama::acoustics::Path frontPath;
	frontPath.reachedListener_ = true;
	frontPath.energy_ = 1.0f;
	frontPath.propagationTime_ = 0.005f;
	frontPath.arrivalDir_ = {
		0.0f, -1.0f, 0.0f
	};

	kodama::acoustics::Path leftPath;
	leftPath.reachedListener_ = true;
	leftPath.energy_ = 1.0f;
	leftPath.propagationTime_ = 0.010f;
	leftPath.arrivalDir_ = {
		1.0f, 0.0f, 0.0f
	};

	kodama::acoustics::Path rightPath;
	rightPath.reachedListener_ = true;
	rightPath.energy_ = 1.0f;
	rightPath.propagationTime_ = 0.015f;
	rightPath.arrivalDir_ = {
		-1.0f, 0.0f, 0.0f
	};

	std::vector<kodama::acoustics::Path> paths{
		frontPath,
		leftPath,
		rightPath
	};

	kodama::acoustics::Listener listener{
	{ 0.0f, 0.0f, 0.0f }
	};

	kodama::acoustics::Response response;

	constexpr std::size_t sampleRate = 48000;

	const auto stereo =
		response.buildStereo(
			paths,
			sampleRate,
			listener);

	for (std::size_t i = 0;
		i < stereo.left_.size();
		++i)
	{
		if (stereo.left_[i] == 0.0f &&
			stereo.right_[i] == 0.0f)
		{
			continue;
		}

		std::cout
			<< "Sample " << i
			<< " | Left: " << stereo.left_[i]
			<< " | Right: " << stereo.right_[i]
			<< '\n';
	}*/

	//////////////////////////////////////////
	/*std::vector<float> samples{
		1.0f
	};

	kodama::audio::Buffer input(
		std::move(samples),
		48000,
		1);

	kodama::audio::ImpulseResponse response;

	response.left_ = {
		0.7f,
		0.0f,
		0.3f
	};

	response.right_ = {
		0.3f,
		0.0f,
		0.1f
	};

	kodama::audio::Convolver convolver;

	const auto output =
		convolver.processStereo(
			input,
			response);

	for (std::size_t i = 0;
		i < output.frameCount();
		++i)
	{
		const auto& samples = output.samples();

		std::cout
			<< "Frame " << i
			<< " | Left: " << samples[i * 2]
			<< " | Right: " << samples[i * 2 + 1]
			<< '\n';
	}*/

	//////////////////////////////////////////

	//kodama::acoustics::Listener listener{
	//	{ 0.0f, 0.0f, 0.0f }
	//};

	//kodama::acoustics::Path front;
	//front.arrivalDir_ = {
	//	0.0f, -1.0f, 0.0f
	//};

	//kodama::acoustics::Path right;
	//right.arrivalDir_ = {
	//	-1.0f, 0.0f, 0.0f
	//};

	//kodama::acoustics::Path left;
	//left.arrivalDir_ = {
	//	1.0f, 0.0f, 0.0f
	//};

	//kodama::acoustics::Path above;
	//above.arrivalDir_ = {
	//	0.0f, -1.0f, -1.0f
	//};

	//kodama::audio::Spatializer spatializer;

	//const auto frontAngles =
	//spatializer.computeAngles(
	//	front,
	//	listener);

	//const auto rightAngles =
	//spatializer.computeAngles(
	//	right,
	//	listener);

	//const auto leftAngles =
	//spatializer.computeAngles(
	//	left,
	//	listener);

	//std::cout
	//<< "Front: "
	//<< frontAngles.first << ", "
	//<< frontAngles.second << '\n';

	//std::cout
	//<< "Right: "
	//<< rightAngles.first << ", "
	//<< rightAngles.second << '\n';

	//std::cout
	//<< "Left: "
	//<< leftAngles.first << ", "
	//<< leftAngles.second << '\n';
//	kodama::acoustics::Listener listener{
//		{ 0.0f, 0.0f, 0.0f }
//	};
//kodama::audio::HRTFDatabase database;
//
//database.add({
//	0.0f,  // Azimuth
//	0.0f,  // Elevation
//	{
//		{1.0f}, // Left HRIR
//		{1.0f}, // Right HRIR
//		48000   // Sample rate
//	}
//	});
//
//database.add({
//	90.0f,  // Azimuth
//	0.0f,   // Elevation
//	{
//		{0.2f}, // Left HRIR
//		{1.0f}, // Right HRIR
//		48000   // Sample rate
//	}
//	});
//kodama::acoustics::Path path;
//path.arrivalDir_ = { 1.0f, 0.0f, 0.0f };
//
//const auto& hrtf =
//kodama::audio::Spatializer::selectHRTF(path, listener, database);
//
//std::cout << "Selected left HRIR: "
//<< hrtf.left_[0] << '\n';
//
//std::cout << "Selected right HRIR: "
//<< hrtf.right_[0] << '\n';

	//kodama::audio::SofaHRTFProvider measuredHRTF;
	//std::string err;

	//if (!measuredHRTF.load("NEECK_HRIR_measured.sofa", 48000, err))
	//{
	//	std::cerr << err << '\n';
	//	return 1;
	//}



	//// Query front and right-side directions.

	//constexpr std::size_t sampleRate = 48000;
	//constexpr std::size_t inputFrames = 4800; // 0.1 seconds
	//constexpr float frequency = 440.0f;
	//constexpr float pi = 3.14159265358979323846f;

	//// Generate a low-amplitude sine wave for testing.
	//std::vector<float> inputSamples(inputFrames);

	//for (std::size_t i = 0; i < inputFrames; ++i)
	//{
	//	const float time = static_cast<float>(i) / sampleRate;

	//	inputSamples[i] = 0.25f * std::sin(2.0f * pi * frequency * time);
	//}

	//kodama::audio::Buffer monoInput(
	//	std::move(inputSamples),
	//	sampleRate,
	//	1);

	//kodama::audio::HRTFDatabase database;

	//kodama::audio::HRTFEntry entry
	//{
	//	0.f,
	//	0.f,
	//	{
	//		{ 1.f, 0.5f },
	//		{ 1.f, 0.5f },
	//		sampleRate
	//	}
	//};

	//entry.hrtf_.leftDelaySec_ = 1.f / sampleRate;
	//entry.hrtf_.rightDelaySec_ = 0.f;

	//// HRTF for a sound directly in front.
	//database.add(std::move(entry));


	//kodama::acoustics::Listener listener;

	//kodama::acoustics::Path frontPath;
	//frontPath.arrivalDir_ = { 0.0f, -1.0f, 0.0f };
	//frontPath.propagationTime_ = 240.0f / sampleRate;
	//frontPath.energy_ = 0.25f;
	//frontPath.reachedListener_ = true;

	//kodama::acoustics::Path rightPath;
	//rightPath.arrivalDir_ = { -1.0f, 0.0f, 0.0f };
	//rightPath.propagationTime_ = 241.0f / sampleRate;
	//rightPath.energy_ = 0.16f;
	//rightPath.reachedListener_ = true;

	//const std::vector<kodama::acoustics::Path> paths = {
	//	frontPath,
	//	rightPath
	//};

	//const auto response = kodama::acoustics::Response::buildBinaural(
	//	paths, sampleRate, listener, database);

	//for (std::size_t i = 240; i <= 242; ++i)
	//{
	//	std::cout << "Sample " << i
	//		<< " | L: " << response.left_[i]
	//		<< " | R: " << response.right_[i]
	//		<< '\n';
	//}
	//const auto stereoOutput =
	//kodama::audio::Convolver::processStereo(monoInput, response);

	//std::cout << "Sample rate: "
	//<< stereoOutput.sampleRate() << '\n';

	//std::cout << "Channels: "
	//<< stereoOutput.channelCount() << '\n';

	//std::cout << "Frames: "
	//<< stereoOutput.frameCount() << '\n';

	//// Inspect the samples around the first path's arrival.
	//const auto& samples = stereoOutput.samples();

	//for (std::size_t frame = 238; frame <= 245; ++frame)
	//{
	//	std::cout << "Frame " << frame
	//		<< " | L: " << samples[frame * 2]
	//		<< " | R: " << samples[frame * 2 + 1]
	//		<< '\n';
	//}

	//const auto front = measuredHRTF.getHRTF(0.0f, 0.0f);
	//const auto right = measuredHRTF.getHRTF(90.0f, 0.0f);
	//const auto left = measuredHRTF.getHRTF(-90.0f, 0.0f);
	//const auto above = measuredHRTF.getHRTF(0.0f, 90.0f);

	//auto printHRTF = [](const char* name, const kodama::audio::HRTF& hrtf)
	//	{
	//		std::cout << name << '\n';

	//		std::cout << "  Left delay: "
	//			<< hrtf.leftDelaySec_ << " s\n";

	//		std::cout << "  Right delay: "
	//			<< hrtf.rightDelaySec_ << " s\n";

	//		std::cout << "  Left first tap: "
	//			<< hrtf.left_[0] << '\n';

	//		std::cout << "  Right first tap: "
	//			<< hrtf.right_[0] << '\n';
	//	};

	//printHRTF("Front", front);
	//printHRTF("Right", right);
	//printHRTF("Left", left);
	//printHRTF("Above", above);


	// Test zero fractional delay.
	const auto zeroDelay =
		kodama::acoustics::makeFractionalDelayCoefficients(0.0);

	const std::array<float, 4> expectedZero = {
		1.0f, 0.0f, 0.0f, 0.0f
	};

	// Test half-sample delay.
	const auto halfDelay =
		kodama::acoustics::makeFractionalDelayCoefficients(0.5);

	const std::array<float, 4> expectedHalf = {
		0.3125f, 0.9375f, -0.3125f, 0.0625f
	};

	constexpr float tolerance = 1.e-5f;

	for (std::size_t i = 0; i < 4; ++i)
	{
		assert(std::abs(zeroDelay[i] - expectedZero[i]) < tolerance);
		assert(std::abs(halfDelay[i] - expectedHalf[i]) < tolerance);
	}

	std::cout << "Fractional-delay coefficient tests passed.\n";

	constexpr std::size_t sampleRate = 48000;

	kodama::audio::HRTFEntry entry{};

	entry.azimuth_ = 0.0f;
	entry.elevation_ = 0.0f;

	entry.hrtf_.left_ = { 1.0f };
	entry.hrtf_.right_ = { 1.0f };
	entry.hrtf_.sampleRate_ = sampleRate;

	entry.hrtf_.leftDelaySec_ = 0.0f;
	entry.hrtf_.rightDelaySec_ =
		0.5f / static_cast<float>(sampleRate);

	kodama::audio::HRTFDatabase database;
	database.add(entry);

	kodama::acoustics::Listener listener;

	kodama::acoustics::Path path;

	path.arrivalDir_ = { 0.0f, -1.0f, 0.0f };
	path.propagationTime_ = 0.0f;
	path.energy_ = 1.0f;
	path.reachedListener_ = true;

	const std::vector<kodama::acoustics::Path> paths = { path };

	const auto response = kodama::acoustics::Response::buildBinaural(
		paths,
		sampleRate,
		listener,
		database);

	for (std::size_t i = 0; i < 4; ++i)
	{
		std::cout << "Sample " << i
			<< " | L: " << response.left_[i]
			<< " | R: " << response.right_[i]
			<< '\n';
	}

	kodama::audio::SofaHRTFProvider measuredHRTF;
	std::string err;

	measuredHRTF.load((assetDir / "HRIR_FULL2DEG.sofa").string(), 48000, err);

	const auto front = measuredHRTF.getHRTF(0.0f, 0.0f);
	const auto right = measuredHRTF.getHRTF(90.0f, 0.0f);
	const auto left = measuredHRTF.getHRTF(-90.0f, 0.0f);
	const auto above = measuredHRTF.getHRTF(0.0f, 45.0f);

	auto inspectHRTF = [](const char* name, const kodama::audio::HRTF& hrtf)
		{
			float leftPeak = 0.0f;
			float rightPeak = 0.0f;

			std::size_t leftNonZero = 0;
			std::size_t rightNonZero = 0;

			for (float sample : hrtf.left_)
			{
				assert(std::isfinite(sample));

				leftPeak = std::max(leftPeak, std::abs(sample));

				if (sample != 0.0f)
					++leftNonZero;
			}

			for (float sample : hrtf.right_)
			{
				assert(std::isfinite(sample));

				rightPeak = std::max(rightPeak, std::abs(sample));

				if (sample != 0.0f)
					++rightNonZero;
			}

			std::cout << "\n" << name << '\n';

			std::cout << "Left peak: " << leftPeak << '\n';
			std::cout << "Right peak: " << rightPeak << '\n';

			std::cout << "Left nonzero samples: "
				<< leftNonZero << '\n';

			std::cout << "Right nonzero samples: "
				<< rightNonZero << '\n';

			std::cout << "Filter length: "
				<< hrtf.left_.size() << '\n';
		};

	inspectHRTF("Front HRTF", front);

	inspectHRTF(
		"Right HRTF",
		measuredHRTF.getHRTF(90.0f, 0.0f));

	auto printHRTF = [](const char* name, const kodama::audio::HRTF& hrtf)
		{
			std::cout << "\n" << name << '\n';

			std::cout << "Sample rate: " << hrtf.sampleRate_ << '\n';
			std::cout << "Filter length: " << hrtf.left_.size() << '\n';

			std::cout << "Left delay: "
				<< hrtf.leftDelaySec_ << " s\n";

			std::cout << "Right delay: "
				<< hrtf.rightDelaySec_ << " s\n";
		};

	printHRTF("Front", front);
	printHRTF("Right", right);
	printHRTF("Left", left);
	printHRTF("Above", above);

	kodama::acoustics::Path measuredPath;

	measuredPath.arrivalDir_ = { 0.0f, -1.0f, 0.0f };
	measuredPath.propagationTime_ = 0.005f;
	measuredPath.energy_ = 0.25f;
	measuredPath.reachedListener_ = true;

	const std::vector<kodama::acoustics::Path> measuredPaths = {
		measuredPath
	};

	const auto measuredResponse =
		kodama::acoustics::Response::buildBinaural(
			measuredPaths,
			sampleRate,
			listener,
			measuredHRTF);

	float peak = 0.0f;

	for (float sample : measuredResponse.left_)
	{
		assert(std::isfinite(sample));
		peak = std::max(peak, std::abs(sample));
	}

	for (float sample : measuredResponse.right_)
	{
		assert(std::isfinite(sample));
		peak = std::max(peak, std::abs(sample));
	}

	std::cout << "sampleRate=" << sampleRate << '\n';

	const auto probe = measuredHRTF.getHRTF(0.0f, 0.0f);
	std::cout << "probe len=" << probe.left_.size() << '\n';

	std::cout << "response L size=" << measuredResponse.left_.size()
		<< " R size=" << measuredResponse.right_.size() << '\n';

	assert(peak > 0.0f);

	std::cout << "Measured binaural response peak: "
		<< peak << '\n';

	constexpr std::size_t durationFrames = sampleRate / 2;
	constexpr std::size_t fadeFrames = sampleRate / 100; // 10 ms

	std::vector<float> inputSamples(durationFrames);

	std::mt19937 randomEngine(12345);
	std::uniform_real_distribution<float> noise(-0.15f, 0.15f);

	for (std::size_t i = 0; i < durationFrames; ++i)
	{
		// Apply a fade at both ends to avoid abrupt clicks.
		const float fadeIn =
			std::min(1.0f, static_cast<float>(i) / fadeFrames);

		const float fadeOut =
			std::min(1.0f, static_cast<float>(
				durationFrames - 1 - i) / fadeFrames);

		const float envelope = std::min(fadeIn, fadeOut);

		inputSamples[i] = noise(randomEngine) * envelope;
	}

	kodama::audio::Buffer monoInput(
		std::move(inputSamples),
		sampleRate,
		1);

	struct DirectionTest
	{
		const char* name;
		kodama::math::Vec3 toSource_;
	};

	const std::array<DirectionTest, 6> directions = { {
		{"front",  { 0.0f,  1.0f,  0.0f}},
		{"right",  { 1.0f,  0.0f,  0.0f}},
		{"left",   {-1.0f,  0.0f,  0.0f}},
		{"behind", { 0.0f, -1.0f,  0.0f}},
		{"above",  { 0.0f,  0.0f,  1.0f}},
		{"below",  { 0.0f,  0.0f, -1.0f}}
	} };

	struct Probe { const char* name; float az; float el; };

	const Probe probes[] = {
		{"front",    0.f,    0.f},
		{"back+",    180.f,  0.f},
		{"back-",   -180.f,  0.f},
		{"back L",  -170.f,  0.f},
		{"back R",   170.f,  0.f},
		{"down",     0.f,   -90.f},
		{"down30L", -90.f,  -30.f},
		{"down30R",  90.f,  -30.f},
	};

	for (const auto& p : probes)
	{
		const auto h = measuredHRTF.getHRTF(p.az, p.el);

		float pl = 0.f, pr = 0.f;
		for (float s : h.left_)  pl = std::max(pl, std::abs(s));
		for (float s : h.right_) pr = std::max(pr, std::abs(s));

		std::cout << p.name << "  az=" << p.az << " el=" << p.el
			<< "  L=" << pl << " R=" << pr << '\n';
	}

	measuredHRTF.dumpNearest(-1.f, 0.f, 0.f);   // directly behind
	measuredHRTF.dumpNearest(0.f, 0.f, -1.f);   // directly below
	measuredHRTF.dumpNearest(1.f, 0.f, 0.f);

	std::vector<std::pair<std::string, kodama::audio::Buffer>> outputs;

	for (const auto& direction : directions)
	{
		kodama::acoustics::Path path;

		// Arrival direction points toward the listener.
		path.arrivalDir_ = -direction.toSource_;

		path.propagationTime_ = 0.005f;
		path.energy_ = 0.5f;
		path.reachedListener_ = true;

		const std::vector<kodama::acoustics::Path> paths = { path };

		const auto response =
			kodama::acoustics::Response::buildBinaural(
				paths,
				sampleRate,
				listener,
				measuredHRTF);

		auto output =
			kodama::audio::Convolver::processStereo(monoInput, response);

		outputs.emplace_back(direction.name, std::move(output));
	}

	kodama::acoustics::Path frontPath;
	frontPath.arrivalDir_ = { 0.0f, -1.0f, 0.0f };
	frontPath.propagationTime_ = 0.005f;
	frontPath.energy_ = 0.50f;
	frontPath.reachedListener_ = true;

	kodama::acoustics::Path rightPath;
	rightPath.arrivalDir_ = { -1.0f, 0.0f, 0.0f };
	rightPath.propagationTime_ = 0.008f;
	rightPath.energy_ = 0.20f;
	rightPath.reachedListener_ = true;

	kodama::acoustics::Path leftPath;
	leftPath.arrivalDir_ = { 1.0f, 0.0f, 0.0f };
	leftPath.propagationTime_ = 0.012f;
	leftPath.energy_ = 0.10f;
	leftPath.reachedListener_ = true;

	const std::vector<kodama::acoustics::Path> multiplePaths = {
		frontPath,
		rightPath,
		leftPath
	};

	const auto multipleResponse =
		kodama::acoustics::Response::buildBinaural(
			multiplePaths,
			sampleRate,
			listener,
			measuredHRTF);

	auto sceneOutput =
		kodama::audio::Convolver::processStereo(
			monoInput,
			multipleResponse);

	outputs.emplace_back(
		"multiple_paths",
		std::move(sceneOutput));

	float globalPeak = 0.0f;

	for (const auto& [name, output] : outputs)
	{
		if (output.channelCount() != 2)
		{
			throw std::runtime_error(
				"Expected stereo output.");
		}

		for (float sample : output.samples())
		{
			if (!std::isfinite(sample))
			{
				throw std::runtime_error(
					"Non-finite audio sample detected.");
			}

			globalPeak = std::max(
				globalPeak, std::abs(sample));
		}
	}

	if (globalPeak <= 0.0f)
	{
		throw std::runtime_error(
			"All generated audio outputs are silent.");
	}

	// Leave 20% headroom and do not boost quiet outputs.
	const float playbackGain =
		std::min(1.0f, 0.8f / globalPeak);

	std::cout << "Global peak: " << globalPeak << '\n';
	std::cout << "Playback gain: " << playbackGain << '\n';

	std::filesystem::create_directories(outputDir);
	// Export all files using the same gain.
	for (const auto& [name, output] : outputs)
	{
		const std::string fileName =
			std::string("hrtf_") + name + ".wav";

		kodama::writer::writeWav16((outputDir / fileName).string(), output, playbackGain);

		std::cout << "Written: " << fileName << '\n';
	}

	return 0;
}