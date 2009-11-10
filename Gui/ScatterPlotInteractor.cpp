#include "ScatterPlotInteractor.h"

#include "Rendering/ScatterPlotRenderer.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotInteractor::ScatterPlotInteractor(ScatterPlotRenderer *renderer)
		: renderer(renderer)
	{
	}

	// ------------------------------------------------------------------------
	ScatterPlotInteractor::~ScatterPlotInteractor()
	{
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MouseMoveEvent(NQVTK::MouseEvent event)
	{
		return Superclass::MouseMoveEvent(event);
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MousePressEvent(NQVTK::MouseEvent event)
	{
		return Superclass::MousePressEvent(event);
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MouseReleaseEvent(NQVTK::MouseEvent event)
	{
		return Superclass::MouseReleaseEvent(event);
	}
}
