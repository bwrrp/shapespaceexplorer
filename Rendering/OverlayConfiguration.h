#pragma once

#include "EvolutionConfiguration.h"

namespace Diverse
{
	class OverlayConfiguration : public EvolutionConfiguration
	{
	public:
		OverlayConfiguration();
		virtual ~OverlayConfiguration();

		virtual void SetupSlices(std::vector<EvolutionSlice> &slices, 
			const ShapeTrajectory *trajectory);
	};
}
