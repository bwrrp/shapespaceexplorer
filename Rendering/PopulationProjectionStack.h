#pragma once

#include "ShapeStack.h"

#include <itpp/itbase.h>

namespace Diverse
{
	class PopulationProjectionStack : public ShapeStack
	{
	public:
		typedef ShapeStack Superclass;

		PopulationProjectionStack(ShapeMesh *mesh);

		bool SetVector(itpp::vec vector);

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);

	protected:
		itpp::vec vector;
	};
}
