#include "ShapeStack.h"

#include "Data/ShapeMesh.h"
#include "Data/Population.h"

#include <itpp/itbase.h>

#include <QDebug>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeStack::ShapeStack(ShapeMesh *mesh) : mesh(mesh), population(0)
	{
	}

	// ------------------------------------------------------------------------
	ShapeStack::~ShapeStack()
	{
		// The mesh and population are deleted by their owners
		// NOTE: Make sure the mesh isn't deleted before the stack!
	}

	// ------------------------------------------------------------------------
	bool ShapeStack::SetPopulation(Population *population)
	{
		if (population && population->GetShapeSpaceDimension() != 
			mesh->GetShapeSpaceDimension())
		{
			qDebug("Shape space dimensions don't match!");
			return false;
		}

		this->population = population;
		return true;
	}

	// ------------------------------------------------------------------------
	ShapeMesh *ShapeStack::GetMesh()
	{
		return mesh;
	}

	// ------------------------------------------------------------------------
	int ShapeStack::GetNumberOfSlices()
	{
		return 1;
	}

	// ------------------------------------------------------------------------
	double ShapeStack::GetSliceOffset(int i)
	{
		return 0.0;
	}

	// ------------------------------------------------------------------------
	void ShapeStack::SetupSliceMesh(int i)
	{
		// The default implementation only contains the mean
		itpp::vec shape(mesh->GetShapeSpaceDimension());
		shape.zeros();
		mesh->SetShape(shape);
	}
}
