#include "ShapeModel.h"
#include "ShapeModel.moc"

#include "ShapeMesh.h"
#include "Population.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeModel::ShapeModel(ShapeMesh *mesh) : mesh(mesh), population(0)
	{
	}

	// ------------------------------------------------------------------------
	ShapeModel::~ShapeModel()
	{
		delete population;
		delete mesh;
	}

	// ------------------------------------------------------------------------
	bool ShapeModel::SetPopulation(Population *population)
	{
		if (population && population->GetShapeSpaceDimension() != 
			mesh->GetShapeSpaceDimension())
		{
			qDebug("Shape space dimensions don't match!");
			return false;
		}

		delete this->population;
		this->population = population;
		// TODO: start PCA computation (on background thread?)
		return true;
	}

	// ------------------------------------------------------------------------
	Population *ShapeModel::GetPopulation()
	{
		return population;
	}

	// ------------------------------------------------------------------------
	ShapeMesh *ShapeModel::GetMesh()
	{
		return mesh;
	}
}
