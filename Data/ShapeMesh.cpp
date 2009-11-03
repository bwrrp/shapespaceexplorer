#include <GLBlaat/GL.h>

#include "ShapeMesh.h"

#include <NQVTK/Renderables/AttributeSet.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeMesh::ShapeMesh(vtkPolyData *meanMesh)
		: NQVTK::PolyData(meanMesh)
	{
	}

	// ------------------------------------------------------------------------
	ShapeMesh::~ShapeMesh()
	{
	}

	// ------------------------------------------------------------------------
	void ShapeMesh::SetShape(const itpp::vec &shape)
	{
		// Create an attributeset for the shape vector
		NQVTK::AttributeSet *shapeOffsets = 
			new NQVTK::AttributeSet(GL_DOUBLE, 3);
		shapeOffsets->SetData(shape.size() / 3, (void*)shape._data());
		// This will delete the old AttributeSet
		// TODO: if slow, try re-using the same set and specifying r/w usage
		AddAttributeSet("shape", shapeOffsets, true);
	}
}
