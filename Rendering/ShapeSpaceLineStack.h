#pragma once

#include "ShapeStack.h"

namespace Diverse
{
	class ShapeSpaceLineStack : public ShapeStack
	{
	public:
		typedef ShapeStack Superclass;

		ShapeSpaceLineStack(ShapeModel *model);

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);
	};
}
