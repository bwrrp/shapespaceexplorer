#include "ScatterPlotInteractor.h"

#include "Rendering/ScatterPlotRenderer.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotInteractor::ScatterPlotInteractor(ScatterPlotRenderer *renderer)
		: renderer(renderer), activeWidget(0)
	{
	}

	// ------------------------------------------------------------------------
	ScatterPlotInteractor::~ScatterPlotInteractor()
	{
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MouseMoveEvent(NQVTK::MouseEvent event)
	{
		bool update = false;
		if (event.buttons & NQVTK::MouseEvent::LeftButton)
		{
			if (activeWidget)
			{
				// TODO: origin needs special treatment
				activeWidget->pos = renderer->ViewportToPos(event.x, event.y);
				activeWidget->state = PointWidget::Drag;
				update = true;
			}
		}
		else if (event.buttons & NQVTK::MouseEvent::RightButton)
		{
			double delta = 1.0 + static_cast<double>(event.y - lastY) * 0.01;
			if (delta < 0.01) delta = 0.01;
			if (delta > 100.0) delta = 100.0;
			renderer->zoom *= delta;
			if (renderer->zoom < 0.0001) renderer->zoom = 0.0001;
			update = true;
		}
		else
		{
			PointWidget *widget = renderer->WidgetAtPoint(event.x, event.y);
			if (widget)
			{
				widget->state = PointWidget::Hover;
				update = true;
			}
			if (widget != activeWidget)
			{
				if (activeWidget)
				{
					activeWidget->state = PointWidget::Normal;
					update = true;
				}
				activeWidget = widget;
			}
		}
		lastX = event.x;
		lastY = event.y;
		return update;
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MousePressEvent(NQVTK::MouseEvent event)
	{
		return MouseMoveEvent(event);
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MouseReleaseEvent(NQVTK::MouseEvent event)
	{
		return MouseMoveEvent(event);
	}
}
