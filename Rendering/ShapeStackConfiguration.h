#pragma once

#include "EvolutionConfiguration.h"

namespace Diverse
{
	class ShapeStackConfiguration : public EvolutionConfiguration
	{
	public:
		ShapeStackConfiguration();
		virtual ~ShapeStackConfiguration();

		virtual void SetupSlices(std::vector<EvolutionSlice> &slices, 
			const ShapeTrajectory *trajectory);
	};
}
