#pragma once

#include "ShapeTrajectory.h"

namespace Diverse
{
	class Population;

	class LinearPopulationTrajectory : public ShapeTrajectory
	{
	public:
		LinearPopulationTrajectory(Population *population);
		virtual ~LinearPopulationTrajectory();

		void SetVector(const itpp::vec &vector);

		virtual void GetShapes(itpp::vec &offsets, itpp::mat &shapes) const;

	protected:
		Population *population;
		itpp::vec vector;
	};
}
