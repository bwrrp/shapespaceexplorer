#pragma once

#include <NQVTK/Rendering/Renderer.h>

namespace Diverse
{
	class ScatterPlotRenderer : public NQVTK::Renderer
	{
	public:
		ScatterPlotRenderer();
		virtual ~ScatterPlotRenderer();

		virtual void Draw();
	};
}
