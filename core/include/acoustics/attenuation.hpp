#pragma once

// Medium
#include <acoustics/medium.hpp>

// stds
#include <cmath>
#include <numbers>


namespace kodama::acoustics
{
	/**
	 * Computes how much sound energy remains after
	 * travelling a given distance
	 * 
	 * [param] dist
	 * 
	 * 
	 * [param] refDist
	 * 
	 * 
	 * [return] 
	 */
	inline float distanceAttenuation(float dist, float refDist)
	{
		// Prevents division by zero, 
		// and prevents attenuation to become greater than ref value
		constexpr float minDist = 1.e-3f;

		// Prevent a case where refDist == 0 if too close to listener
		refDist = std::fmax(refDist, minDist);

		// Prevent distance from being below refDist
		dist = std::fmax(dist, refDist);

		// Source has full reference strength at refDist
		// If greater than refDist, intensity falls
		const float ratio = refDist / dist;

		return ratio * ratio;
	}

	/**
	 * Computes the Stokes attenuation coefficient
	 * 
	 * [param] freqHz
	 * Frequency of the sound
	 * 
	 * [param] medium
	 * Medium traversed
	 * 
	 * [return] 
	 * Attenuation coefficient
	 * 
	 */
	inline float stokesAttenuationCoefficient(
		float freqHz, 
		const AcousticMedium& medium)
	{
		// Convert frequency Hz to angular frequency
		const float omega = 2.f * std::numbers::pi_v<float> * freqHz;

		// alpha = (2 * eta * omega^2) / (3 * rho * c^3)
		return
			(2.f * medium.dynamicViscosity_ * omega * omega) /
			(3.f * medium.density_ *
				medium.speedOfSound_ *
				medium.speedOfSound_ *
				medium.speedOfSound_);
	}

	/**
	 * Computes the Stokes Kirchhoff attenuation coefficient
	 *
	 * [param] freqHz
	 * Frequency of the sound
	 *
	 * [param] medium
	 * Medium traversed
	 *
	 * [return]
	 * Attenuation coefficient
	 *
	 */
	inline float stokesKirchhoffAttenuationCoefficient(
		float freqHz,
		const AcousticMedium& medium)
	{
		// Convert frequency Hz to angular frequency
		const float omega = 2.f * std::numbers::pi_v<float> * freqHz;

		// Viscous contribution
		// (4/3) * eta
		const float viscousTerm = (4.f / 3.f) * medium.dynamicViscosity_;

		// Thermal contribution
		// k * (1/Cv - 1/Cp)
		const float thermalTerm = medium.thermalConductivity_ *
			(
				(1.f / medium.heatCapacityCv_) - (1.f / medium.heatCapacityCp_)
				);

		const float bulkViscosityTerm = medium.bulkViscosity_;

		// alpha = (omega^2) / (2 * rho * c^3) * (viscous + thermal + bulk) 
		return (omega * omega)
			/

			(2.f * medium.density_ *
			medium.speedOfSound_ *
			medium.speedOfSound_ *
			medium.speedOfSound_
			)
			*

			(viscousTerm + thermalTerm + bulkViscosityTerm);
	}

	/**
	 * Computes the remaining amplitude from 
	 * travelling through a medium with a given distance
	 * based on Stokes
	 * 
	 * [param] dist
	 * Distance travelled
	 * 
	 * [param] freqHz
	 * Frequency of the sound
	 * 
	 * [param] medium
	 * Medium traversed
	 * 
	 * [return] 
	 * Remaining amplitude
	 * 
	 */
	inline float amplitudeAttenuation(
		float dist, 
		float freqHz, 
		const AcousticMedium& medium)
	{
		const float alpha = stokesKirchhoffAttenuationCoefficient(freqHz, medium);

		// Pressure/amplitude ratio after travelling the distance:
		// A(d) / A(0) = exp(-alpha * d)
		return std::exp(-alpha * dist);
	}

	/**
	 * Computes the remaining energy after travelling through the medium.
	 * 
	 * [param] dist
	 * Distance travelled
	 * 
	 * [param] freqHz
	 * Frequency of the sound
	 * 
	 * [param] medium
	 * Medium traversed
	 * 
	 * [return] 
	 * Remaining energy
	 * 
	 */
	inline float energyAttenuation(
		float dist,
		float freqHz,
		const AcousticMedium& medium)
	{
		const float amplitudeFactor =
			amplitudeAttenuation(
				dist,
				freqHz,
				medium);

		// Intensity is proportional to the square of pressure
		// amplitude so I(d) / I(0) = [A(d) / A(0)]^2
		return amplitudeFactor * amplitudeFactor;
	}

}
