#pragma once

#include "ShapeStack.h"

namespace Diverse
{
	class PopulationProjectionStack : public ShapeStack
	{
	public:
		typedef ShapeStack Superclass;

		PopulationProjectionStack(ShapeModel *model);

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);
	};
}
