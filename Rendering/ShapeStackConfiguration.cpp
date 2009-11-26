#include "ShapeStackConfiguration.h"

#include "Data/ShapeTrajectory.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeStackConfiguration::ShapeStackConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	ShapeStackConfiguration::~ShapeStackConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	void ShapeStackConfiguration::SetupSlices(
		std::vector<EvolutionSlice> &slices, const ShapeTrajectory *trajectory)
	{
		assert(trajectory != 0);
		itpp::vec offsets;
		itpp::mat shapes;
		trajectory->GetShapes(offsets, shapes);
		int numSlices = offsets.size();
		slices.resize(numSlices);

		// Rescale offsets to a nice -5..5 range
		double shift = -offsets(0);
		double scale = 10.0 / (offsets(numSlices - 1) - offsets(0));

		// Generate shape stack
		for (int i = 0; i < numSlices; ++i)
		{
			EvolutionSlice &slice = slices[i];
			slice.origin = NQVTK::Vector3(0.0, 0.0, 
				scale * (offsets(i) + shift) - 5.0);
			slice.right = NQVTK::Vector3(5.0, 0.0, 0.0);
			slice.up = NQVTK::Vector3(0.0, 5.0, 0.0);
			slice.shape = shapes.get_row(i);
		}

		// Set up bounds
		bounds[0] = -5.0;
		bounds[1] = 5.0;
		bounds[2] = -5.0;
		bounds[3] = 5.0;
		bounds[4] = -5.0;
		bounds[5] = 5.0;
	}
}
