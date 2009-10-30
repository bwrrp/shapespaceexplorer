#pragma once

#include <NQVTK/Renderables/PolyData.h>

class vtkPolyData;

namespace Diverse
{
	class ShapeModel : public NQVTK::PolyData
	{
	public:
		typedef NQVTK::PolyData Superclass;

		ShapeModel(vtkPolyData *meanMesh);
		virtual ~ShapeModel();

		// TODO: add a way to set the shape vector
		// this will be stored as a custom attribute GLBuffer
	};
}
