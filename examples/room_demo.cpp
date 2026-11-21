#include <acoustics/propagator.hpp>
#include <acoustics/response.hpp>
#include <audio/Sofa_HRTF_provider.hpp>
#include <audio/convolver.hpp>
#include <material/material_reg.hpp>
#include <scene/scene.hpp>
#include <writer/wav_writer.hpp>

// stds

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <filesystem>

using namespace kodama;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cerr << "usage: room_demo <hrir.sofa>\n";
		return 1;
	}

	constexpr size_t sampleRate = 48000;

	// Materials: absorption 0 = perfect mirror, 1 = perfect absorber
	materials::Registry materials;
	const size_t plaster = materials.registerMaterial({ "plaster", 0.1f });

	// A 6 x 4 x 3 m room (X = width, Y = depth, Z = height) made of triangles
	scene::Scene room;

	const auto addQuad = [&](math::Vec3 a, math::Vec3 b, math::Vec3 c, math::Vec3 d)
		{
			room.addTriangle({ a, b, c, plaster });
			room.addTriangle({ a, c, d, plaster });
		};

	addQuad({ 0, 0, 0 }, { 6, 0, 0 }, { 6, 4, 0 }, { 0, 4, 0 }); // floor
	addQuad({ 0, 0, 3 }, { 6, 0, 3 }, { 6, 4, 3 }, { 0, 4, 3 }); // ceiling
	addQuad({ 0, 0, 0 }, { 6, 0, 0 }, { 6, 0, 3 }, { 0, 0, 3 }); // front wall
	addQuad({ 0, 4, 0 }, { 6, 4, 0 }, { 6, 4, 3 }, { 0, 4, 3 }); // back wall
	addQuad({ 0, 0, 0 }, { 0, 4, 0 }, { 0, 4, 3 }, { 0, 0, 3 }); // left wall
	addQuad({ 6, 0, 0 }, { 6, 4, 0 }, { 6, 4, 3 }, { 6, 0, 3 }); // right wall

	// Source strength is the energy at referenceDist_ metres from the source
	const acoustics::SoundSource source{ { 1.f, 1.f, 1.5f }, 1.f, 1.f };

	// The listener is a sphere that catches rays (see "How it works")
	acoustics::Listener listener;
	listener.pos_ = { 5.f, 3.f, 1.5f };
	listener.radius_ = 0.5f;

	// Fire rays from the source and keep the ones that reach the listener
	acoustics::Propagator propagator;
	const acoustics::AcousticMedium air;

	const auto paths = propagator.propagateMany(
		source, listener, room, materials, air,
		200000,   // rays
		20);      // max bounces per ray

	std::cout << paths.size() << " rays reached the listener\n";

	// Turn the paths into a left/right impulse response using measured HRTFs
	audio::SofaHRTFProvider hrtf;
	std::string error;

	if (!hrtf.load(argv[1], sampleRate, error))
	{
		std::cerr << error << '\n';
		return 1;
	}

	const auto ir = acoustics::Response::buildBinaural(paths, sampleRate, listener, hrtf);

	// A short burst of decaying noise as the "dry" sound
	std::mt19937 gen(1);
	std::uniform_real_distribution<float> noise(-1.f, 1.f);

	std::vector<float> dry(sampleRate / 4);
	for (size_t i = 0; i < dry.size(); ++i)
		dry[i] = noise(gen) * std::exp(-12.f * static_cast<float>(i) / static_cast<float>(dry.size()));

	const audio::Buffer mono(dry, sampleRate, 1);

	// Put the sound in the room, then write it out normalized
	audio::Buffer wet = audio::Convolver::processStereo(mono, ir);

	float peak = 0.f;
	for (float s : wet.samples())
		peak = std::max(peak, std::fabs(s));

	const std::filesystem::path outputDir = KODAMA_OUTPUT_DIR;
	std::filesystem::create_directories(outputDir);

	const std::string outPath = (outputDir / "room.wav").string();
	writer::writeWav16(outPath, wet, peak > 0.f ? 0.9f / peak : 1.f);

	std::cout << "wrote " << outPath << " (" << wet.duration() << " s)\n";
	return 0;
}
