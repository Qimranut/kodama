#pragma once

// Vec3
#include <math/vec3.hpp>

// stds
#include <vector>
#include <cmath>
#include <cstddef>
#include <numbers>


namespace kodama::acoustics
{
	class RayGenerator
	{
	public:

		/**
		 * .
		 * 
		 * [param] rayCount
		 * [return] 
		 */
		static std::vector<math::Vec3> generateDirections(size_t rayCount)
		{
			std::vector<math::Vec3> directions;

			directions.reserve(rayCount);

			constexpr float goldenAngle = 2.39996323f;

			for (size_t i = 0; i < rayCount; i++)
			{
				// Move from +1 to -1 across the sphere
				//
				// Using (i + 0.5) avoids putting rays 
				// directly on the poles of the sphere
				const float t =
					(static_cast<float>(i) + 0.5f) /
					static_cast<float>(rayCount);

				const float z = 1.f - 2.f * t;

				// Radius of the circle
				const float radius = std::sqrt(1.f - z * z);

				// Rotate around the sphere using the golden angle
				const float theta = std::fmod(goldenAngle * static_cast<float>(i), 2.f * std::numbers::pi_v<float>);

				directions.push_back(
					{
						radius * std::cos(theta),
						radius * std::sin(theta),
						z
					});
			}

			return directions;
		}
	};
}
