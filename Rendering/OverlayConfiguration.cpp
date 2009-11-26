#include "OverlayConfiguration.h"

#include "Data/ShapeTrajectory.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	OverlayConfiguration::OverlayConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	OverlayConfiguration::~OverlayConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	void OverlayConfiguration::SetupSlices(
		std::vector<EvolutionSlice> &slices, const ShapeTrajectory *trajectory)
	{
		assert(trajectory != 0);
		itpp::vec offsets;
		itpp::mat shapes;
		trajectory->GetShapes(offsets, shapes);
		int numSlices = offsets.size();
		slices.resize(numSlices);

		// Generate overlapping slices
		for (int i = 0; i < numSlices; ++i)
		{
			EvolutionSlice &slice = slices[i];
			slice.origin = NQVTK::Vector3(0.0, 0.0, 0.0);
			slice.right = NQVTK::Vector3(5.0, 0.0, 0.0);
			slice.up = NQVTK::Vector3(0.0, 5.0, 0.0);
			slice.shape = shapes.get_row(i);
		}

		// Set up bounds
		bounds[0] = -5.0;
		bounds[1] = 5.0;
		bounds[2] = -5.0;
		bounds[3] = 5.0;
		bounds[4] = -1.0;
		bounds[5] = 1.0;
	}
}
