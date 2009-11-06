#include "ShapeStack.h"

#include "Data/ShapeMesh.h"
#include "Data/Population.h"

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
}
