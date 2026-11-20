#pragma once

namespace kodama::acoustics
{
	// Represents the substance which 
	// the sound wave is currently propagating through
	struct AcousticMedium
	{
		// Dynamic viscosity of the medium
		// Unit: Pa.s (kg / (m.s))
		float dynamicViscosity_{ 1.81e-5f };

		// Density of medium
		// Unit: kg/m^3
		float density_{ 1.225f };

		// Approximate speed of sound through air 
		// Unit: m/s
		float speedOfSound_{ 343.f };

		// Thermal conductivity
		// Unit: W/(m.K)
		float thermalConductivity_{ 0.0262f };

		// Specific heat capacity at constant pressure
		// Unit: J/(kg.K)
		float heatCapacityCp_{ 1005.f };

		// Specific heat capacity at constant volume
		// Unit: J/(kg.K)
		float heatCapacityCv_{ 718.f };

		// Bulk viscosity
		// Unit: Pa.s
		float bulkViscosity_{ 0.f };
	};

}
