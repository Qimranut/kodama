#pragma once

// Path
#include <acoustics/path.hpp>

// Listener
#include <acoustics/listener.hpp>

// Stereo gains
#include <audio/stereo_gains.hpp>

// HRTF
#include <audio/HRTF_database.hpp>

// stds
#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>


namespace kodama::audio
{
	class Spatializer
	{
	public:

		/**
		 * Computes stereo gains for an arriving acoustic path
		 * 
		 * [param] path
		 * Acoustic path reaching the listener
		 * 
		 * [param] listener
		 * Listener orientation used for positioning
		 * 
		 * [return] 
		 * Equal-power left/right gains for the path
		 * 
		 */
		StereoGains computeGains(
			const acoustics::Path& path, 
			const acoustics::Listener& listener) const
		{
			// Get the opposite direction of the arrival direction
			// to find out where the sound is coming from
			const math::Vec3 toSource = -path.arrivalDir_;

			const math::Vec3 listenerRight = listener.right();

			// Project the source direction onto the listener's left/right axis
			// -1 -> completely left
			//  0 -> centered
			// +1 -> completely right
			const float pan = std::clamp(math::dot(toSource, listenerRight), -1.f, 1.f);

			// Convert the [-1, 1] pan range into [0, pi/2]
			// Equal power panning uses sine/cosine so that perceived loudness remains
			// approximately constant as the sound moves between channels
			const float angle = (pan + 1.f) * std::numbers::pi_v<float> / 4.f;

			const float leftGain = std::clamp(std::cos(angle), 0.f, 1.f);
			const float rightGain = std::clamp(std::sin(angle), 0.f, 1.f);

			return {leftGain, rightGain };
		}

		/**
		 * Converts a world-space direction into listener-relative
		 * azimuth and elevation
		 * 
		 * [param] toSource
		 * Direction where the sound is coming from
		 * 
		 * [param] listener
		 * Listener orientation
		 * 
		 * [return] 
		 * Azimuth and Elevation in degrees
		 * 
		 */
		static std::pair<float, float> computeAngles(
			const math::Vec3& toSource,
			const acoustics::Listener& listener)
		{
			// Convert world-space direction into listener-local coords
			const float x = math::dot(toSource, listener.right());
			const float y = math::dot(toSource, listener.forward_);
			const float z = math::dot(toSource, listener.up_);

			// Horizontal angular pos
			//
			// atan2(right, forward)
			// 0	= front
			// +90	= right
			// -90	= left
			const float azimuth = std::atan2(x, y) * 180.f / std::numbers::pi_v<float>;

			// Horizontal distance from the listener
			const float horizontalDist = std::sqrt(x * x + y * y);

			// Vertical angular position
			const float elevation = std::atan2(z, horizontalDist) * 180.f / std::numbers::pi_v<float>;

			return { azimuth, elevation };
		}

		static const HRTF& selectHRTF(
			const acoustics::Path& path,
			const acoustics::Listener& listener,
			const HRTFDatabase& database)
		{
			// Convert the path's arrival direction into 
			// the listener's local azimuth/elevation
			const auto [azimuth, elevation] = computeAngles(-path.arrivalDir_, listener);

			// Find closest HRTF for that direction
			return database.getHRTF(azimuth, elevation);
		}
	};
}
