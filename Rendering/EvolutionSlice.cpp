#include <GLBlaat/GL.h>

#include "EvolutionSlice.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	EvolutionSlice EvolutionSlice::Interpolate(const EvolutionSlice &slice1, 
		const EvolutionSlice &slice2, double t)
	{
		EvolutionSlice result;
		double invt = 1.0 - t;

		// Slice parameters
		result.shape = invt * slice1.shape + t * slice2.shape;
		result.offset = invt * slice1.offset + t * slice2.offset;

		result.contourColoring = invt * slice1.contourColoring + 
			t * slice2.contourColoring;
		result.contourOpacity = invt * slice1.contourOpacity + 
			t * slice2.contourOpacity;
		result.shapeOpacity = invt * slice1.shapeOpacity + 
			t * slice2.shapeOpacity;

		// Slice position and orientation
		result.origin = invt * slice1.origin + t * slice2.origin;
		// Not quite spherical linear interpolation, but close enough...
		result.up = (invt * slice1.up + t * slice2.up).normalized() * 
			(invt * slice1.up.length() + t * slice2.up.length());
		result.right = (invt * slice1.right + t * slice2.right).normalized() * 
			(invt * slice1.right.length() + t * slice2.right.length());
		return result;
	}

	// ------------------------------------------------------------------------
	void EvolutionSlice::Draw() const
	{
		glBegin(GL_QUADS);
		glTexCoord2d(1.0, 0.0);
		glVertex3dv((origin - right - up).V);
		glTexCoord2d(1.0, 1.0);
		glVertex3dv((origin - right + up).V);
		glTexCoord2d(0.0, 1.0);
		glVertex3dv((origin + right + up).V);
		glTexCoord2d(0.0, 0.0);
		glVertex3dv((origin + right - up).V);
		glEnd();
	}
}
