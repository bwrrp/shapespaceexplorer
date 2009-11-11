#include "ShapeSpaceLineStack.h"

#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeSpaceLineStack::ShapeSpaceLineStack(ShapeModel *model) 
		: ShapeStack(model)
	{
	}

	// ------------------------------------------------------------------------
	int ShapeSpaceLineStack::GetNumberOfSlices()
	{
		// TODO: may want to make this adjustable
		return 90;
	}

	// ------------------------------------------------------------------------
	double ShapeSpaceLineStack::GetSliceOffset(int i)
	{
		return static_cast<double>(i) / 30.0 - 1.5;
	}

	// ------------------------------------------------------------------------
	void ShapeSpaceLineStack::SetupSliceMesh(int i)
	{
		// TODO: add offset to allow lines not through the origin (mean shape)
		itpp::vec shape(model->GetMesh()->GetShapeSpaceDimension());
		shape.zeros();
		shape = vector * GetSliceOffset(i);
		model->GetMesh()->SetShape(shape);
	}
}
