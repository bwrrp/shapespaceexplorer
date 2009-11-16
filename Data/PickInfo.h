#pragma once

#include <NQVTK/Math/Vector3.h>

#include <vector>

class vtkDelaunay2D;

namespace Diverse
{
	// Helper for picking and interpolation between 2D unstructured points
	class PickInfo
	{
	public:
		PickInfo();
		~PickInfo();

		// Update the lookup structure for the given set of points
		void UpdateInfo(const std::vector<NQVTK::Vector3> &points);

		// Find triangle points and barycentric coords for the given point
		bool Pick(const NQVTK::Vector3 &point, 
			int &id1, int &id2, int &id3, double &u, double &v);

	protected:
		vtkDelaunay2D *filter;
	};
}
