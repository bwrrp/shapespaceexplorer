#include "ScatterPlotInteractor.h"
#include "ScatterPlotInteractor.moc"

#include "Rendering/ScatterPlotRenderer.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	void ScatterPlotMessenger::EmitCursorPosChanged(int x, int y)
	{
		emit CursorPosChanged(x, y);
	}

	// ------------------------------------------------------------------------
	void ScatterPlotMessenger::EmitProjectionChanged()
	{
		emit ProjectionChanged();
	}

	// ------------------------------------------------------------------------
	ScatterPlotInteractor::ScatterPlotInteractor(ScatterPlotRenderer *renderer)
		: renderer(renderer), activeWidget(0)
	{
		messenger = new ScatterPlotMessenger();
	}

	// ------------------------------------------------------------------------
	ScatterPlotInteractor::~ScatterPlotInteractor()
	{
		delete messenger;
	}

	// ------------------------------------------------------------------------
	ScatterPlotMessenger *ScatterPlotInteractor::GetMessenger()
	{
		return messenger;
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotInteractor::MouseMoveEvent(NQVTK::MouseEvent event)
	{
		bool update = false;
		if (event.buttons & NQVTK::MouseEvent::LeftButton)
		{
			// Dragging a node
			if (activeWidget)
			{
				// TODO: origin needs special treatment
				activeWidget->pos = renderer->ViewportToPos(event.x, event.y);
				activeWidget->state = PointWidget::Drag;
				update = true;
				messenger->EmitProjectionChanged();
				// Recompute the picking structure
				renderer->UpdatePickInfo();
			}
		}
		else if (event.buttons & NQVTK::MouseEvent::RightButton)
		{
			// Zooming
			double delta = 1.0 + static_cast<double>(lastY - event.y) * 0.01;
			if (delta < 0.01) delta = 0.01;
			if (delta > 100.0) delta = 100.0;
			renderer->zoom *= delta;
			if (renderer->zoom < 0.0001) renderer->zoom = 0.0001;
			update = true;
			messenger->EmitProjectionChanged();
		}
		else
		{
			// Hovering
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
		messenger->EmitCursorPosChanged(event.x, event.y);
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
