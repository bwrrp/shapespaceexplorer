#pragma once

#include <NQVTK/Renderables/PolyData.h>

#include <itpp/itbase.h>

class vtkPolyData;

namespace Diverse
{
	class ShapeMesh : public NQVTK::PolyData
	{
	public:
		typedef NQVTK::PolyData Superclass;

		ShapeMesh(vtkPolyData *meanMesh);
		virtual ~ShapeMesh();

		void SetShape(const itpp::vec &shape);
	};
}
