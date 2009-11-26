#pragma once

#include "EvolutionSlice.h"

#include <vector>

namespace Diverse
{
	class ShapeTrajectory;

	class EvolutionConfiguration
	{
	public:
		EvolutionConfiguration();
		virtual ~EvolutionConfiguration();

		virtual void SetupSlices(std::vector<EvolutionSlice> &slices, 
			const ShapeTrajectory *trajectory) = 0;

		virtual double* GetBounds();

	protected:
		double bounds[6];
	};
}
