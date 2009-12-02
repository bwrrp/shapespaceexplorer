#include "SideBySideConfiguration.h"

#include "Data/ShapeTrajectory.h"

#include <cassert>
#include <cmath>

namespace Diverse
{
	// ------------------------------------------------------------------------
	SideBySideConfiguration::SideBySideConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	SideBySideConfiguration::~SideBySideConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	void SideBySideConfiguration::SetupSlices(
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

		// Generate grid of slices
		// TODO: this should probably match the aspect ratio
		double N = static_cast<double>(numSlices);
		double W = ceil(2.0 * sqrt(N / 2.0));
		double H = ceil(N / W);
		double size = 10.0 / W;
		int width = static_cast<int>(W);
		int height = static_cast<int>(H);
		for (int j = 0; j < height; ++j)
		{
			double y = static_cast<double>(j);
			for (int i = 0; i < width; ++i)
			{
				int index = width * j + i;
				if (index < numSlices)
				{
					double x = static_cast<double>(i);
					EvolutionSlice &slice = slices[index];
					slice.origin = NQVTK::Vector3(
						((W - 1.0) / 2.0 - x) * size, 
						((H - 1.0) / 2.0 - y) * size, 
						0.0);
					slice.right = NQVTK::Vector3(size * 0.5, 0.0, 0.0);
					slice.up = NQVTK::Vector3(0.0, size * 0.5, 0.0);
					slice.shape = shapes.get_row(index);
					slice.offset = scale * offsets(index);
				}
			}
		}
		bounds[0] = -(W - 1.0) / 2.0 * size;
		bounds[1] = (W - 1.0) / 2.0 * size;
		bounds[2] = -(H - 1.0) / 2.0 * size;
		bounds[3] = (H - 1.0) / 2.0 * size;
		bounds[4] = -1.0;
		bounds[5] = 1.0;
	}
}
