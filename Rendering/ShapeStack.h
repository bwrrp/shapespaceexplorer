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

	protected:
		ShapeMesh *mesh;
		Population *population;
	};
}
