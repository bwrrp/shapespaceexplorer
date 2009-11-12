#include <GLBlaat/GL.h>

#include "ShapeMesh.h"

#include <NQVTK/Renderables/AttributeSet.h>

#include <QFileInfo>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeMesh::ShapeMesh(vtkPolyData *meanMesh)
		: NQVTK::PolyData(meanMesh)
	{
		numPoints = meanMesh->GetNumberOfPoints();
		shapeAttr = new NQVTK::AttributeSet(GL_DOUBLE, 3);
		shapeAttr->SetData(numPoints, 0, GL_DYNAMIC_DRAW);
		AddAttributeSet("shape", shapeAttr, true);
	}

	// ------------------------------------------------------------------------
	ShapeMesh::~ShapeMesh()
	{
	}

	// ------------------------------------------------------------------------
	ShapeMesh *ShapeMesh::Load(const QString &filename)
	{
		QFileInfo fi(filename);
		if (fi.suffix() == "vtp")
		{
			// Use VTK to load the mesh
			vtkSmartPointer<vtkXMLPolyDataReader> reader = 
				vtkSmartPointer<vtkXMLPolyDataReader>::New();
			reader->SetFileName(filename.toUtf8());
			vtkSmartPointer<vtkTriangleFilter> tri = 
				vtkSmartPointer<vtkTriangleFilter>::New();
			tri->SetInputConnection(reader->GetOutputPort());
			tri->Update();
			return new ShapeMesh(tri->GetOutput());
		}
		else
		{
			qDebug("Unsupported mesh file type!");
			return 0;
		}
	}

	// ------------------------------------------------------------------------
	void ShapeMesh::SetShape(const itpp::vec &shape)
	{
		assert(shape.size() == GetShapeSpaceDimension());
		shapeAttr->SetData(shape.size() / 3, (void*)shape._data(), 
			GL_DYNAMIC_DRAW);
	}

	// ------------------------------------------------------------------------
	int ShapeMesh::GetShapeSpaceDimension()
	{
		return numPoints * 3;
	}
}
