#pragma once

// Ray
#include <geometry/ray.hpp>

// Vec3
#include <math/vec3.hpp>

// stds
#include <cstddef>
#include <limits>


namespace kodama::geometry
{
	struct Triangle
	{
		math::Vec3 v0_;
		math::Vec3 v1_;
		math::Vec3 v2_;

		// ID of material to assign
		size_t matID_{ std::numeric_limits<size_t>::max() };

		// Triangle Normal
		math::Vec3 normal() const
		{
			return math::normalize(math::cross(v1_ - v0_, v2_ - v0_));
		}

		/**
		 * Makes use of the Möller–Trumbore algorithm to determine
		 * an intersection between a ray and triangle
		 * 
		 * [param] ray
		 * [param] dist
		 * 
		 * [return] 
		 * Returns true if there is an intersection
		 */
		bool intersect(const Ray& ray, float& distance) const
		{
			// Define edges originating from v0_
			const math::Vec3 edge1 = v1_ - v0_;
			const math::Vec3 edge2 = v2_ - v0_;

			// Determine whether ray is parallel to triangle's plane
			const math::Vec3 h = math::cross(ray.dir_, edge2);

			const float a = math::dot(edge1, h);

			constexpr float epsilon = 0.000001f;

			// If a is close to zero, 
			// ray is effectively parallel to triangle's plane
			if (std::fabs(a) < epsilon)
				return false;

			// Used to avoid repeatedly dividing by a
			const float f = 1.f / a;

			// Vector from first triangle vertex to ray origin
			const math::Vec3 s = ray.origin_ - v0_;

			// First barycentric coordinate
			const float u = f * math::dot(s, h);

			// Intersection with triangle check
			if (u < 0.f || u > 1.f)
				return false;
			
			const math::Vec3 q = math::cross(s, edge1);

			const float v = f * math::dot(ray.dir_, q);

			// Intersection with triangle check
			if (v < 0.f || u + v > 1.f)
				return false;

			distance = f * math::dot(edge2, q);

			// Reject tiny distances to prevent self intersections
			return distance >= 1e-4f;
		}
	};
}
