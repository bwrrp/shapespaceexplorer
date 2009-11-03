#include <GLBlaat/GL.h>

#include "ShapeMesh.h"

#include <NQVTK/Renderables/AttributeSet.h>

#include <QFileInfo>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkXMLPolyDataReader.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeMesh::ShapeMesh(vtkPolyData *meanMesh)
		: NQVTK::PolyData(meanMesh)
	{
		NQVTK::AttributeSet *shapeOffsets = 
			new NQVTK::AttributeSet(GL_DOUBLE, 3);
		shapeOffsets->SetData(meanMesh->GetNumberOfPoints(), 0);
		AddAttributeSet("shape", shapeOffsets, true);
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
		// Create an attributeset for the shape vector
		NQVTK::AttributeSet *shapeOffsets = 
			new NQVTK::AttributeSet(GL_DOUBLE, 3);
		shapeOffsets->SetData(shape.size() / 3, (void*)shape._data());
		// TODO: if slow, try re-using the same set and specifying r/w usage
		NQVTK::AttributeSet *oldSet = ReplaceAttributeSet(
			"shape", shapeOffsets);
		delete oldSet;
	}
}
