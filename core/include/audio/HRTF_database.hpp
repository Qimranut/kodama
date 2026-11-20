#pragma once

// HRTF entry
#include <audio/HRTF_entry.hpp>

// stds
#include <cmath>
#include <limits>
#include <vector>
#include <stdexcept>


namespace kodama::audio
{
	class HRTFDatabase
	{
	public:

		/**
		 * Adds a directional HRTF to the database
		 * 
		 * [param] entry
		 * Direction and corresponding HRTF
		 * 
		 */
		void add(HRTFEntry entry)
		{
			entries_.push_back(std::move(entry));
		}

		/**
		 * Retrieves closest stored HRTF for a direction.
		 * Using the same name as the function under Sofa_HRTF_provider.hpp
		 * to help with templating
		 * 
		 * [param] azimuth
		 * Desired horizontal direction in degrees
		 *
		 * [param] elevation
		 * Desired vertical direction in degrees
		 * 
		 * [return] 
		 * Returns a reference to the closest stored HRTF
		 * 
		 */
		const HRTF& getHRTF(float azimuthDeg, float elevationDeg) const
		{
			return getClosest(azimuthDeg, elevationDeg);
		}

	private:

		std::vector<HRTFEntry> entries_;

		/**
		 * Find the HRTF whose direction is closest to the
		 * requested direction
		 *
		 * [param] azimuth
		 * Desired horizontal direction in degrees
		 *
		 * [param] elevation
		 * Desired vertical direction in degrees
		 *
		 * [return]
		 * Nearest directional HRTF
		 *
		 */
		const HRTF& getClosest(
			float azimuth,
			float elevation) const
		{
			if (entries_.empty())
				throw std::runtime_error("HRTF database is empty");

			const HRTFEntry* closest = &entries_.front();
			float closestDist = std::numeric_limits<float>::max();

			for (const auto& entry : entries_)
			{
				const float deltaAzimuth = std::fmod(entry.azimuth_ - azimuth + 540.f, 360.f) - 180.f;

				const float deltaElevation = entry.elevation_ - elevation;

				// Compute Euclidean distance in angle space
				const float distance = deltaAzimuth * deltaAzimuth + deltaElevation * deltaElevation;
				// Can scale the azimuth term
				// deltaAzimuth * std::cos(elevation * pi / 180.f)

				if (distance < closestDist)
				{
					closestDist = distance;
					closest = &entry;
				}
			}

			return closest->hrtf_;
		}
	};
}
