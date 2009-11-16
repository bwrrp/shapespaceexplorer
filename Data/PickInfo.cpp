#include "PickInfo.h"

#include <vtkIdList.h>
#include <vtkDelaunay2D.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	PickInfo::PickInfo()
	{
		filter = vtkDelaunay2D::New();
	}

	// ------------------------------------------------------------------------
	PickInfo::~PickInfo()
	{
		if (filter) filter->Delete();
	}

	// ------------------------------------------------------------------------
	void PickInfo::UpdateInfo(const std::vector<NQVTK::Vector3> &points)
	{
		// Create a polydata containing the points
		vtkSmartPointer<vtkPolyData> input = 
			vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPoints> inpoints = 
			vtkSmartPointer<vtkPoints>::New();
		int numPoints = points.size();
		inpoints->SetNumberOfPoints(numPoints);
		for (int i = 0; i < numPoints; ++i)
		{
			inpoints->SetPoint(i, points[i].V);
		}
		input->SetPoints(inpoints);
		// Run vtkDelaunay2D on these points
		filter->SetInput(input);
		filter->Update();
	}

	// ------------------------------------------------------------------------
	bool PickInfo::Pick(const NQVTK::Vector3 &point, 
		int &id1, int &id2, int &id3, double &u, double &v)
	{
		vtkPolyData *output = filter->GetOutput();
		assert(output != 0);

		// For each cell...
		int numCells = output->GetNumberOfCells();
		for (int i = 0; i < numCells; ++i)
		{
			// Get cell points
			vtkSmartPointer<vtkIdList> pids = 
				vtkSmartPointer<vtkIdList>::New();
			output->GetCellPoints(i, pids);
			assert(pids->GetNumberOfIds() == 3);
			NQVTK::Vector3 a, b, c;
			output->GetPoint(pids->GetId(0), a.V);
			output->GetPoint(pids->GetId(1), b.V);
			output->GetPoint(pids->GetId(2), c.V);

			// Test if the point is in this cell
			// From http://www.blackpawn.com/texts/pointinpoly/default.html
			NQVTK::Vector3 v0 = c - a;
			NQVTK::Vector3 v1 = b - a;
			NQVTK::Vector3 v2 = point - a;

			// Compute dot products
			double dot00 = v0.dot(v0);
			double dot01 = v0.dot(v1);
			double dot02 = v0.dot(v2);
			double dot11 = v1.dot(v1);
			double dot12 = v1.dot(v2);

			// Compute barycentric coordinates
			double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
			double uu = (dot11 * dot02 - dot01 * dot12) * invDenom;
			double vv = (dot00 * dot12 - dot01 * dot02) * invDenom;

			// Check if point is in triangle
			if (uu > 0.0 && vv > 0.0 && uu + vv < 1.0)
			{
				id1 = pids->GetId(0);
				id2 = pids->GetId(1);
				id3 = pids->GetId(2);
				u = uu;
				v = vv;
				return true;
			}
		}
		// TODO: return closest for points outside the triangulation?
		return false;
	}
}
