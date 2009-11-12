#pragma once

#include "ScatterPlotRenderer.h"

class GLProgram;
class GLFramebuffer;

namespace NQVTK
{
	class Scene;
	class SimpleRenderer;
}

namespace Diverse
{
	class ShapeMesh;

	class ScatterShapesRenderer : public ScatterPlotRenderer
	{
	public:
		typedef ScatterPlotRenderer Superclass;

		ScatterShapesRenderer();
		virtual ~ScatterShapesRenderer();

		void SetMesh(ShapeMesh *mesh);

	protected:
		virtual bool Initialize();

		virtual void DrawPoint(const itpp::vec &point);

		ShapeMesh *mesh;

		GLFramebuffer *meshBuffer;
		NQVTK::SimpleRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;
	};
}
