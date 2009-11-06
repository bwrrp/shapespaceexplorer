#pragma once

namespace Diverse
{
	class ShapeMesh;
	class Population;

	class ShapeStack
	{
	public:
		ShapeStack(ShapeMesh *mesh);
		virtual ~ShapeStack();

		bool SetPopulation(Population *population);

		ShapeMesh *GetMesh();

		virtual int GetNumberOfSlices();
		virtual double GetSliceOffset(int i);
		virtual void SetupSliceMesh(int i);

		// TODO: add a way to communicate additional shading parameters 
		// for the mesh and composite shaders

	protected:
		ShapeMesh *mesh;
		Population *population;
	};
}
