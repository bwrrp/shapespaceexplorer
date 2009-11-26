#pragma once

#include "EvolutionConfiguration.h"

namespace Diverse
{
	class SideBySideConfiguration : public EvolutionConfiguration
	{
	public:
		SideBySideConfiguration();
		virtual ~SideBySideConfiguration();

		virtual void SetupSlices(std::vector<EvolutionSlice> &slices, 
			const ShapeTrajectory *trajectory);
	};
}
