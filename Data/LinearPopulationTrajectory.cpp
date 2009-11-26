#include "LinearPopulationTrajectory.h"

#include "Population.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	LinearPopulationTrajectory::LinearPopulationTrajectory(
		Population *population) : population(population)
	{
		assert(population != 0);

		vector.set_size(population->GetShapeSpaceDimension());
		vector.zeros();
		vector(0) = 1.0;
	}

	// ------------------------------------------------------------------------
	LinearPopulationTrajectory::~LinearPopulationTrajectory()
	{
	}

	// ------------------------------------------------------------------------
	void LinearPopulationTrajectory::SetVector(const itpp::vec &vector)
	{
		assert(vector.size() == population->GetShapeSpaceDimension());
		this->vector = vector;
	}

	// ------------------------------------------------------------------------
	void LinearPopulationTrajectory::GetShapes(
		itpp::vec &offsets, itpp::mat &shapes) const
	{
		int numIndividuals = population->GetNumberOfIndividuals();
		itpp::vec unsortedOffsets(numIndividuals);
		itpp::mat unsortedShapes(numIndividuals, 
			population->GetShapeSpaceDimension());
		for (int i = 0; i < numIndividuals; ++i)
		{
			itpp::vec indiv = population->GetIndividual(i);
			unsortedOffsets(i) = itpp::dot(indiv, vector);
			unsortedShapes.set_row(i, indiv);
		}

		// Sort by offset
		itpp::ivec order = itpp::sort_index(unsortedOffsets);
		offsets.set_size(numIndividuals);
		shapes.set_size(numIndividuals, population->GetShapeSpaceDimension());
		for (int i = 0; i < numIndividuals; ++i)
		{
			offsets(i) = unsortedOffsets(order(i));
			shapes.set_row(i, unsortedShapes.get_row(order(i)));
		}
	}
}
