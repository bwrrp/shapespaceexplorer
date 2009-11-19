#pragma once

#include <NQVTK/Renderables/PolyData.h>

#include <itpp/itbase.h>

#include <QString>

class vtkPolyData;

namespace NQVTK { class AttributeSet; }

namespace Diverse
{
	class ShapeMesh : public NQVTK::PolyData
	{
	public:
		typedef NQVTK::PolyData Superclass;

		static ShapeMesh *Load(const QString &filename);
		virtual ~ShapeMesh();

		void SetShape(const itpp::vec &shape);
		void SetReference(const itpp::vec &shape);

		int GetShapeSpaceDimension();

	protected:
		ShapeMesh(vtkPolyData *meanMesh);

		NQVTK::AttributeSet *shapeAttr;
		NQVTK::AttributeSet *refAttr;

		int numPoints;
	};
}
