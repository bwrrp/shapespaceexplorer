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
			this->vector = vector;
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
		return population->GetNumberOfIndividuals();
	}

	// ------------------------------------------------------------------------
	double PopulationProjectionStack::GetSliceOffset(int i)
	{
		// TODO: add offset to allow lines not through the origin (mean shape)
		return itpp::dot(vector, population->GetIndividual(i));
	}

	// ------------------------------------------------------------------------
	void PopulationProjectionStack::SetupSliceMesh(int i)
	{
		mesh->SetShape(population->GetIndividual(i));
	}
}
