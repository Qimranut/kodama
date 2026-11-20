#pragma once

// Vec3
#include <math/vec3.hpp>

// Ray
#include <geometry/ray.hpp>

namespace kodama::geometry
{
	struct Plane
	{
		math::Vec3 point_;
		math::Vec3 normal_;

		/**
		 * Check if a ray casted intersects with the plane.
		 * 
		 * [param] ray
		 * Ray casted
		 * 
		 * [param] distance
		 * Distance between the ray and plane
		 * 
		 * [return] 
		 */
		bool intersect(const Ray& ray, float& distance) const
		{
			const float denominator = math::dot(ray.dir_, normal_);

			// Ray is parallel to plane, intersection impossible
			if (!denominator)
				return false;

			// Determine how far ray's origin is from plane along plane's normal
			distance = math::dot(point_ - ray.origin_, normal_) / denominator;

			// Reject tiny distances to prevent self intersections
			return distance >= 1e-4f;
		}
	};
}
