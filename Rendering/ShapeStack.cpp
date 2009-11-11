#include "ShapeStack.h"

#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

#include <itpp/itbase.h>

#include <QDebug>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeStack::ShapeStack(ShapeModel *model) : model(model)
	{
		assert(model != 0);
	}

	// ------------------------------------------------------------------------
	ShapeStack::~ShapeStack()
	{
	}

	// ------------------------------------------------------------------------
	ShapeMesh *ShapeStack::GetMesh()
	{
		return model->GetMesh();
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
		itpp::vec shape(model->GetMesh()->GetShapeSpaceDimension());
		shape.zeros();
		model->GetMesh()->SetShape(shape);
	}
}
