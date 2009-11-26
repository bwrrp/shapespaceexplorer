#include "LinearMorphTrajectory.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	LinearMorphTrajectory::LinearMorphTrajectory(const itpp::vec &shape1, 
		const itpp::vec &shape2, int numSteps)
		: shape1(shape1), shape2(shape2), numSteps(numSteps)
	{
		assert(shape1.size() == shape2.size());
	}

	// ------------------------------------------------------------------------
	LinearMorphTrajectory::~LinearMorphTrajectory()
	{
	}

	// ------------------------------------------------------------------------
	void LinearMorphTrajectory::GetShapes(itpp::vec &offsets, 
		itpp::mat &shapes) const
	{
		offsets.set_size(numSteps);
		shapes.set_size(numSteps, shape1.size());
		// Linearly interpolate between shape1 and shape2
		for (int i = 0; i < numSteps; ++i)
		{
			double t = static_cast<double>(i) / 
				static_cast<double>(numSteps - 1);
			offsets(i) = t;
			shapes.set_row(i, (1.0 - t) * shape1 + t * shape2);
		}
	}
}
