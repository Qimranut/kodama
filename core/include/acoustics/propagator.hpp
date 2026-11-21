#pragma once

// Ray 
#include <acoustics/ray.hpp>

// Ray Generator
#include <acoustics/ray_generator.hpp>

// Hit result
#include <geometry/hit_result.hpp>

// Path
#include <acoustics/path.hpp>

// Scene
#include <scene/scene.hpp>

// Attenuation
#include "attenuation.hpp"

// Material Registry
#include <material/material_reg.hpp>

// stds
#include <vector>
#include <cstddef>
#include <limits>


namespace kodama::acoustics
{
	inline constexpr float defaultEnergyThreshold{ 1.e-4f };

	class Propagator
	{
	public:

		/**
		 * Propagates rays into the world space
		 * in a spherical manner
		 * 
		 * [param] source
		 * Sound source / Origin of propagation
		 * 
		 * [param] listener
		 * Listener
		 * 
		 * [param] scene
		 * Playing scene
		 * 
		 * [param] reg
		 * Material registry
		 * 
		 * [param] rayCount
		 * Number of rays to propagate
		 * 
		 * [param] maxBounces
		 * Maximum ray bounce count
		 * 
		 * [return] 
		 * Paths of rays which reached the listener
		 * 
		 */
		std::vector<Path> propagateMany(
			const SoundSource& source,
			const Listener& listener,
			const scene::Scene& scene,
			const materials::Registry& reg,
			const AcousticMedium& medium,
			size_t rayCount,
			int maxBounces,
			float energyThreshold = 1.e-4f) const
		{
			std::vector<Path> paths;

			// Generate directions covering the 
			// spherical range of the sound source
			const auto directions = RayGenerator::generateDirections(rayCount);

			for (const auto& dir : directions)
			{
				// Every ray begins at the sound source
				Ray ray { { source.pos_, dir }, source.strength_ };

				const auto path = propagateToListener(
					ray,
					scene,
					reg,
					medium,
					source,
					listener,
					maxBounces,
					energyThreshold,
					rayCount);

				// Keep paths that reached the listener
				if (path.reachedListener_)
					paths.push_back(path);
			}
			
			return paths;
		}

		/**
		 * Finds out if the casted ray intersects teh listener's radius
		 * 
		 * [param] ray
		 * Casted ray
		 * 
		 * [param] listener
		 * Listener 
		 * 
		 * [param] maxDist
		 * Maximum distance along the ray segment 
		 * which is allowed to detect the listener
		 * (due to surface blocks)
		 * 
		 * [param] dist
		 * [return] 
		 */
		bool intersectsListener(
			const geometry::Ray& ray,
			const Listener& listener,
			float maxDist,
			float& dist) const
		{
			const math::Vec3 toListener = listener.pos_ - ray.origin_;

			// Project vector to listener onto ray direction
			// 
			// Indicates where the point on the ray that 
			// is closest to the listener is located
			float closestDist = math::dot(toListener, ray.dir_);

			// Clamp closest point to the current ray segment
			//
			// Do not detect the listener beyond max distance because
			// there may be a surface there first
			if (closestDist < 0.f)
				closestDist = 0.f;

			if (closestDist > maxDist)
				closestDist = maxDist;

			// Calculate the point on the ray closest to listener
			const math::Vec3 closestPoint = ray.origin_ + ray.dir_ * closestDist;

			// Vector from closest point to the listener
			const math::Vec3 diff = listener.pos_ - closestPoint;

			// Find out if the ray is within the radius of the listener
			const float distSquared = math::dot(diff, diff);
			const float radiusSquared = listener.radius_ * listener.radius_;

			if (distSquared <= radiusSquared)
			{
				// Ray passes through listener's capture sphere
				dist = closestDist;
				return true;
			}

			return false;
		}

		/**
		 * Propagates a ray to a listener
		 * 
		 * [param] ray
		 * Casted ray
		 * 
		 * [param] scene
		 * Playing scene
		 * 
		 * [param] reg
		 * Material registry
		 * 
		 * [param] listener
		 * Listener to reach
		 * 
		 * [param] maxBounces
		 * Maximum number of bounces for the ray
		 * 
		 * [param] energyThreshold
		 * Relative energy below which a ray is abandoned
		 * 
		 * [param] emittedRayCount
		 * Total number of rays emitted by the source in this
		 * simulation, used to normalize the energy of a captured path
		 * 
		 * [return] 
		 * Path of the ray
		 * 
		 */
		Path propagateToListener(
			Ray ray,
			const scene::Scene& scene,
			const materials::Registry& reg,
			const AcousticMedium& medium,
			const SoundSource& source,
			const Listener& listener,
			int maxBounces,
			float energyThreshold = defaultEnergyThreshold,
			size_t emittedRayCount = 1) const
		{
			// Record the ray's origin as being part of the sound path
			Path path;
			path.points_.push_back(
				{
					ray.ray_.origin_,
					{},
					materials::InvalidMatID
				});

			for (int bounce = 0; bounce < maxBounces; ++bounce)
			{
				geometry::HitResult hit;

				// Check whether the ray hits any scene geometry
				const bool hitSurface = scene.intersect(ray.ray_, hit);

				// Listener can only be reached before first surface intersection on ray segment
				const float maxDist = hitSurface ? 
					hit.dist_ : 
					std::numeric_limits<float>::max();

				float listenerDist{ 0.f };

				if (intersectsListener(
					ray.ray_,
					listener,
					maxDist,
					listenerDist))
				{
					// Listener was reached before the next surface

					// Distance from the source to listener centre
					const float unfoldedDist = path.totalDist_ + 
						math::length(listener.pos_ - ray.ray_.origin_);

					// Add final segment from ray origin to listener
					path.totalDist_ += listenerDist;

					// Store listener as a point on sound path
					path.points_.push_back(
						{
							listener.pos_,
							{},
							materials::InvalidMatID
						});

					// Convert complete path length into travel time
					// time = distance / speed
					path.propagationTime_ = path.totalDist_ / medium.speedOfSound_;

					// Geometric spreading over the complete path
					const float spreading = distanceAttenuation(unfoldedDist, source.referenceDist_);

					// Capture sphere only intercepts a fraction of the emitted rays.
					// Share source's energy between the rays we expect to be captured, so
					// total is independent of ray count and radius
					const float captureWeight = 1.f /
						(static_cast<float>(emittedRayCount) *
							listenerCaptureFraction(unfoldedDist, listener.radius_));

					// Viscous attenuation through the medium
					const float mediumLoss = energyAttenuation(path.totalDist_, ray.frequency_, medium);

					// Apply distance attenuation using source's reference
					path.energy_ = ray.energy_ *  mediumLoss * spreading * captureWeight;

					// Store the direction the sound was travelling immediately
					// before reaching the listener
					path.arrivalDir_ = ray.ray_.dir_;

					// Path reached listener
					path.reachedListener_ = true;

					return path;
				}

				// Ray did not reach listener,
				// and has nowhere to bounce from
				if (!hitSurface)
					break;

				// Add ray segment to total path length
				path.totalDist_ += hit.dist_;

				// Store reflection point
				path.points_.push_back(
					{
						hit.pos_,
						hit.normal_,
						hit.triangle_->matID_
					});

				// Get surface material
				const auto& mat = reg.material(hit.triangle_->matID_);

				ray.energy_ *= mat.reflectivity();
				
				// Estimate how much energy could still reach the listener
				// Including
				// - Current ray energy after surface absorption
				// - Geometric spreading from the complete path distance
				// - Stokes Kirchhoff attenuation through the medium
				const float remainingEnergy = ray.energy_ *
					distanceAttenuation(path.totalDist_, source.referenceDist_) *
					energyAttenuation(path.totalDist_, ray.frequency_, medium);

				// Convert relative threshold into 
				// an absolute energy value based on this source
				const float minimumEnergy = source.strength_ * energyThreshold;

				// If even the estimated energy is below cutoff, stop tracing this ray
				// as another reflection cannot increase it
				if (remainingEnergy < minimumEnergy)
					break;

				// Calculate reflected direction
				const auto reflectedDir =
					math::normalize(
						math::reflect(
							ray.ray_.dir_,
							hit.normal_)
					);

				// Prevent same surface intersection
				constexpr float epsilon = 1.e-4f;

				ray.ray_.origin_ = hit.pos_ + reflectedDir * epsilon;
				ray.ray_.dir_ = reflectedDir;

				++path.bounceCount_;
			}

			// Path did not reach the listener
			path.energy_ = 0.f;

			return path;
		}

		/**
		 * Checks if a sound source has a direct path to a listener.
		 * 
		 * [param] source
		 * Sound source
		 * 
		 * [param] listener
		 * Sound listener
		 * 
		 * [param] scene
		 * Playing scene
		 * 
		 * [return] 
		 * Returns true if sound source has a direct path to listener.
		 * 
		 */
		bool hasDirectPath(
			const SoundSource& source,
			const Listener& listener,
			const scene::Scene& scene) const
		{
			// Vector from source to listener
			const math::Vec3 toListener = listener.pos_ - source.pos_;

			const float listenerDist = math::length(toListener);

			// Source and listener are on the same pos
			if (listenerDist <= 1.e-6f)
				return true;

			const math::Vec3 dir = math::normalize(toListener);

			geometry::Ray ray
			{
				source.pos_,
				dir
			};

			geometry::HitResult hit;

			// No geometry was hit
			if (!scene.intersect(ray, hit))
				return true;

			// If surface is farther away than the listener,
			// it does not block the direct path.
			//
			// If it is closer, the sound hits the surface first.
			constexpr float epsilon = 1.e-4f;

			return hit.dist_ >= listenerDist - epsilon;
		}

		/**
		 * Simulates a multi-bounce ray.
		 * 
		 * [param] ray
		 * Ray casted
		 * 
		 * [param] scene
		 * Playing Scene
		 * 
		 * [param] matReg
		 * Material registry
		 * 
		 * [param] maxBounces
		 * Max number of bounces of the casted ray
		 * 
		 * [return] 
		 * Ray bouncing result
		 * 
		 */
		Path propagate(
			acoustics::Ray ray,
			const scene::Scene& scene,
			const materials::Registry& matReg,
			int maxBounces) const
		{
			Path path;
			path.points_.push_back(
				{
					ray.ray_.origin_,
					{},
					materials::InvalidMatID
				});

			for (int bounce = 0; bounce < maxBounces; ++bounce)
			{
				geometry::HitResult hit;

				// Find closest surface hit by current ray
				if (!scene.intersect(ray.ray_, hit))
					break;

				// Record distance travelled
				path.totalDist_ += hit.dist_;

				// Record the point where the ray hit the surface
				path.points_.push_back(
					{
						hit.pos_,
						hit.normal_,
						hit.triangle_->matID_
					});

				// Get material of hit triangle
				const auto& mat = matReg.material(hit.triangle_->matID_);

				// Account for sound energy absorption
				ray.energy_ *= mat.reflectivity();

				// Direction after reflection
				const auto reflectedDir = 
					math::normalize(
						math::reflect(
							ray.ray_.dir_,
							hit.normal_)
				);

				// Move the surface away slightly to prevent self-intersections
				constexpr float epsilon = 1.e-4f;

				// Reflected ray begins where the previous ray hit the surface
				ray.ray_.origin_ = hit.pos_ + reflectedDir * epsilon;
				ray.ray_.dir_ = reflectedDir;

				++path.bounceCount_;
			}

			path.energy_ = ray.energy_;

			return path;
		}
	};
}
