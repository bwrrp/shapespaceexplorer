#include "PopulationProjectionStack.h"

#include "Data/ShapeModel.h"
#include "Data/Population.h"
#include "Data/ShapeMesh.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	PopulationProjectionStack::PopulationProjectionStack(ShapeModel *model)
		: ShapeStack(model)
	{
	}

	// ------------------------------------------------------------------------
	int PopulationProjectionStack::GetNumberOfSlices()
	{
		Population *population = model->GetPopulation();
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
		Population *population = model->GetPopulation();
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
		ShapeMesh *mesh = model->GetMesh();
		Population *population = model->GetPopulation();
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
