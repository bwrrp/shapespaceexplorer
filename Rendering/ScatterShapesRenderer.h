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
	class ScatterShapesRenderer : public ScatterPlotRenderer
	{
	public:
		typedef ScatterPlotRenderer Superclass;

		ScatterShapesRenderer();
		virtual ~ScatterShapesRenderer();

	protected:
		virtual bool Initialize();

		virtual void DrawPoint(const itpp::vec &point);

		GLFramebuffer *meshBuffer;
		NQVTK::SimpleRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;
	};
}
