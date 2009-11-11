#pragma once

#include "ShapeStack.h"

#include <itpp/itbase.h>

namespace Diverse
{
	class ShapeSpaceLineStack : public ShapeStack
	{
	public:
		typedef ShapeStack Superclass;

		ShapeSpaceLineStack(ShapeModel *model);

		bool SetVector(itpp::vec vector);

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);

	protected:
		itpp::vec vector;
	};
}
