#pragma once

#include <NQVTK/Math/Vector3.h>

#include <vector>

class GLFramebuffer;
class GLProgram;
class GLOcclusionQuery;

namespace Diverse
{
	class GPUVoronoi
	{
	public:
		GPUVoronoi();
		virtual ~GPUVoronoi();

		bool Initialize();

		bool Resize(int w, int h);

		// Draw the Voronoi diagram for the given points
		void UpdateVoronoi(const std::vector<NQVTK::Vector3> &points);

		// Query natural neighbors interpolation weights for the given point
		std::vector<double> QueryPoint(const NQVTK::Vector3 &point);

	protected:
		// FBO holding the current Voronoi diagram
		GLFramebuffer *voronoiBuffer;
		// Full-screen pass depth cone shader
		GLProgram *coneShader;
		// Queries used for finding point interpolation weights
		std::vector<GLOcclusionQuery*> queries;

		// Buffer size, as floats
		float width;
		float height;

		void UpdateQueries(unsigned int num);

		void PushGLState();
		void PopGLState();

		void DrawConePass(const NQVTK::Vector3 &point);
	};
}
