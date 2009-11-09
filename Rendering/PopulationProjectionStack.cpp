#include "PopulationProjectionStack.h"

#include "Data/Population.h"
#include "Data/ShapeMesh.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	PopulationProjectionStack::PopulationProjectionStack(ShapeMesh *mesh)
		: ShapeStack(mesh)
	{
	}

	// ------------------------------------------------------------------------
	bool PopulationProjectionStack::SetVector(itpp::vec vector)
	{
		if (vector.size() == mesh->GetShapeSpaceDimension())
		{
			// Normalize the vector
			double length = sqrt(itpp::dot(vector, vector));
			this->vector = vector / length;
			// TODO: pre-sort the population to avoid sorting per frame
			return true;
		}
		else
		{
			return false;
		}
	}

	// ------------------------------------------------------------------------
	int PopulationProjectionStack::GetNumberOfSlices()
	{
		if (population)
		{
			return population->GetNumberOfIndividuals();
		}
		else
		{
			return 1;
		}
	}

	// ------------------------------------------------------------------------
	double PopulationProjectionStack::GetSliceOffset(int i)
	{
		if (population)
		{
			// TODO: add offset to allow lines not through the origin
			return itpp::dot(vector, population->GetIndividual(i));
		}
		else
		{
			return 0.0;
		}
	}

	// ------------------------------------------------------------------------
	void PopulationProjectionStack::SetupSliceMesh(int i)
	{
		if (population)
		{
			mesh->SetShape(population->GetIndividual(i));
		}
		else
		{
			itpp::vec shape(mesh->GetShapeSpaceDimension());
			shape.zeros();
			mesh->SetShape(shape);
		}
	}
}
