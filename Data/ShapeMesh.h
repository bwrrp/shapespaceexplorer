#pragma once

#include <NQVTK/Renderables/PolyData.h>

#include <itpp/itbase.h>

#include <QString>

class vtkPolyData;

namespace Diverse
{
	class ShapeMesh : public NQVTK::PolyData
	{
	public:
		typedef NQVTK::PolyData Superclass;

		static ShapeMesh *Load(const QString &filename);
		virtual ~ShapeMesh();

		void SetShape(const itpp::vec &shape);

		int GetShapeSpaceDimension();

	protected:
		ShapeMesh(vtkPolyData *meanMesh);

		int numPoints;
	};
}
