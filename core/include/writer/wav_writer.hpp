#pragma once

// Audio buffer
#include <audio/buffer.hpp>

// stds
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>


namespace kodama::writer
{
	inline void writeWav16(
		const std::string& filePath,
		const kodama::audio::Buffer& buffer,
		float playbackGain = 1.f)
	{
		const auto& samples = buffer.samples();

		const uint32_t channels = static_cast<uint32_t>(buffer.channelCount());

		const uint32_t sampleRate = static_cast<uint32_t>(buffer.sampleRate());

		if (!channels || !sampleRate || !std::isfinite(playbackGain))
			throw std::invalid_argument("Invalid WAV parameters");

		const uint64_t dataSize64 = static_cast<uint64_t>(samples.size() * 2);

		if (dataSize64 > std::numeric_limits<uint32_t>::max())
			throw std::length_error("WAV exceeds standard RIFF size.");

		const auto dataSize = static_cast<uint32_t>(dataSize64);
		const uint32_t blockAlign = channels * 2;
		const uint32_t byteRate = sampleRate * blockAlign;

		std::ofstream file(filePath, std::ios::binary);
		if (!file)
			throw std::runtime_error("Could not open WAV output file.");

		const auto writeU16 = [&file](uint16_t value)
			{
				file.put(static_cast<char>(value));
				file.put(static_cast<char>(value >> 8));
			};

		const auto writeU32 = [&file](uint32_t value)
			{
				for (int i = 0; i < 4; i++)
				{
					file.put(static_cast<char>(value >> (i * 8)));
				}
			};

		file.write("RIFF", 4);
		writeU32(36 + dataSize);
		file.write("WAVE", 4);

		file.write("fmt ", 4);
		writeU32(16);             // PCM format chunk size
		writeU16(1);              // PCM encoding
		writeU16(static_cast<std::uint16_t>(channels));
		writeU32(sampleRate);
		writeU32(byteRate);
		writeU16(static_cast<std::uint16_t>(blockAlign));
		writeU16(16);             // 16 bits per sample

		file.write("data", 4);
		writeU32(dataSize);

		for (float sample : samples)
		{
			const float scaled = std::clamp(
				sample * playbackGain, -1.0f, 1.0f);

			const auto pcm = static_cast<std::int16_t>(
				std::lround(scaled < 0.0f
					? scaled * 32768.0f
					: scaled * 32767.0f));

			writeU16(static_cast<std::uint16_t>(pcm));
		}

		if (!file)
			throw std::runtime_error("Failed while writing WAV file.");
	}
}

