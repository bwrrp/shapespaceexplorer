#pragma once

#include <QObject>

namespace Diverse
{
	class ShapeMesh;
	class Population;

	class ShapeModel : public QObject
	{
		Q_OBJECT;

	public:
		ShapeModel(ShapeMesh *mesh);
		virtual ~ShapeModel();

		bool SetPopulation(Population *population);

		Population *GetPopulation();
		ShapeMesh *GetMesh();

	protected:
		ShapeMesh *mesh;
		Population *population;
	};
}
