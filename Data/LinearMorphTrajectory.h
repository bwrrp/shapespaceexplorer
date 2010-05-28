#pragma once

#include "ShapeTrajectory.h"

namespace Diverse
{
	class LinearMorphTrajectory : public ShapeTrajectory
	{
	public:
		LinearMorphTrajectory(const itpp::vec &shape1, 
			const itpp::vec &shape2, int numSteps = 50);
		virtual ~LinearMorphTrajectory();

		virtual void GetShapes(itpp::vec &offsets, itpp::mat &shapes) const;

		const itpp::vec &GetShape1() { return shape1; }
		const itpp::vec &GetShape2() { return shape2; }

	protected:
		itpp::vec shape1;
		itpp::vec shape2;

		int numSteps;
	};
}
