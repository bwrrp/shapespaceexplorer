#pragma once

#include <itpp/itbase.h>

namespace Diverse
{
	class ShapeModel;
	class ShapeMesh;

	class ShapeStack
	{
	public:
		ShapeStack(ShapeModel *model);
		virtual ~ShapeStack();

		ShapeMesh *GetMesh();

		virtual bool SetVector(itpp::vec vector);

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);

		// TODO: add a way to communicate additional shading parameters 
		// for the mesh and composite shaders

	protected:
		ShapeModel *model;

		itpp::vec vector;
	};
}
