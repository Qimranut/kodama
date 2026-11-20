#pragma once

// Ray
#include <geometry/ray.hpp>

// Triangle
#include <geometry/triangle.hpp>

// Hit Result
#include <geometry/hit_result.hpp>

// Sound Source
#include <acoustics/sound_source.hpp>

// Listener
#include <acoustics/listener.hpp>

// stds
#include <limits>
#include <vector>


namespace kodama::scene
{
	class Scene
	{
	public:

		/**
		 * Populate scene geometry with triangles
		 * 
		 * [param] triangle
		 * Triangle to add to the scene
		 * 
		 */
		void addTriangle(const geometry::Triangle& triangle)
		{
			triangles_.push_back(triangle);
		}

		/**
		 * Tests the ray against every triangle within the scene.
		 * 
		 * [param] ray
		 * Ray cast
		 * 
		 * [param] hit
		 * Closest hit
		 * 
		 * [return]
		 * Returns true if at least 1 triangle was hit.
		 * 
		 */
		bool intersect(const geometry::Ray& ray, geometry::HitResult& hit) const
		{
			// Closest intersection recording
			float closestDist = std::numeric_limits<float>::max();
			const geometry::Triangle* closestTri{ nullptr };

			for (const auto& t : triangles_)
			{
				float curDist = 0.f;
					
				// Test for intersection with current triangle
				if (!t.intersect(ray, curDist))
					continue;

				if (curDist < closestDist)
				{
					closestDist = curDist;
					closestTri = &t;
				}
			}

			if (!closestTri)
				return false;

			// Record hit
			hit.dist_ = closestDist;
			hit.pos_ = ray.origin_ + ray.dir_ * closestDist;
			hit.normal_ = closestTri->normal();
			hit.triangle_ = closestTri;

			return true;
		}

		// Accessors

		// Sound Source
		const acoustics::SoundSource& soundSource() const
		{
			return source_;
		}

		void soundSource(const acoustics::SoundSource& source)
		{
			source_ = source;
		}

		// Listener
		const acoustics::Listener& listener() const
		{
			return listener_;
		}

		void listener(const acoustics::Listener& listener)
		{
			listener_ = listener;
		}

	private:

		std::vector<geometry::Triangle> triangles_;

		acoustics::SoundSource source_{};

		acoustics::Listener listener_{};
	};
}
