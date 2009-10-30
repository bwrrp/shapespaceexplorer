#include <GLBlaat/GL.h>

#include "ShapeModel.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeModel::ShapeModel(vtkPolyData *meanMesh)
		: NQVTK::PolyData(meanMesh)
	{
	}

	// ------------------------------------------------------------------------
	ShapeModel::~ShapeModel()
	{
	}
}
