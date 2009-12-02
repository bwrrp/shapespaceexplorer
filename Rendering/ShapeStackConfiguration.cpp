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

		// Rescale offsets to a nice -1..1 range
		double scale = 1.0 / std::max(
			std::abs(offsets(numSlices - 1)), 
			std::abs(offsets(0)));

		// Generate shape stack
		for (int i = 0; i < numSlices; ++i)
		{
			EvolutionSlice &slice = slices[i];
			slice.origin = NQVTK::Vector3(0.0, 0.0, 
				5.0 * scale * offsets(i));
			slice.right = NQVTK::Vector3(5.0, 0.0, 0.0);
			slice.up = NQVTK::Vector3(0.0, 5.0, 0.0);

			slice.shape = shapes.get_row(i);
			slice.offset = scale * offsets(i);

			float opacity = static_cast<float>(
				std::max(1.0 - std::abs(slice.offset), 0.2));
			slice.contourColoring = 0.0f;
			slice.contourOpacity = 1.0f * opacity;
			slice.shapeOpacity = 0.6f * opacity;
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
