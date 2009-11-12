#define _USE_MATH_DEFINES

#include <GLBlaat/GL.h>

#include "ScatterPlotRenderer.h"

#include "Data/Population.h"

#include <cassert>
#include <cmath>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotRenderer::ScatterPlotRenderer() : population(0)
	{
		widgetRadius = 6.0;
		zoom = 1.0;
		// Add the origin widget
		PointWidget origin = {};
		widgets.push_back(origin);
		// Start with four axes
		SetNumberOfAxes(5);

		// Use some useful initial positions
		// TODO: should be handled by SetNumberOfAxes
		for (unsigned int i = 0; i < widgets.size() - 1; ++i)
		{
			double t = static_cast<double>(i) / 
				static_cast<double>(widgets.size() - 1) * 2.0 * M_PI;
			widgets[i + 1].pos.x = std::cos(t);
			widgets[i + 1].pos.y = std::sin(t);
		}
	}

	// ------------------------------------------------------------------------
	ScatterPlotRenderer::~ScatterPlotRenderer()
	{
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::SetViewport(int x, int y, int w, int h)
	{
		Superclass::SetViewport(x, y, w, h);

		// Determine coordinate system relative to current viewport
		widgetOrigin = NQVTK::Vector3(
			static_cast<double>(viewportX) + 
			static_cast<double>(viewportWidth) / 2.0, 
			static_cast<double>(viewportY) + 
			static_cast<double>(viewportHeight) / 2.0, 
			0.0);
		widgetScale = std::min(static_cast<double>(viewportWidth), 
			static_cast<double>(viewportHeight)) * 0.3;
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::Draw()
	{
		Clear();

		// This is a 2D view, no renderables are supported (for now)
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(viewportX, viewportX + viewportWidth, 
			viewportY + viewportHeight, viewportY, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Draw axes
		glEnable(GL_LINE_SMOOTH);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		NQVTK::Vector3 origin = widgets[0].pos;
		NQVTK::Vector3 vporigin = PosToViewport(origin);
		NQVTK::Vector3 color1(1.0, 0.5, 0.0);
		NQVTK::Vector3 color2(0.0, 0.5, 1.0);
		glLineWidth(2.5);
		glBegin(GL_LINES);
		for (unsigned int i = 1; i < widgets.size(); ++i)
		{
			// TODO: assign each axis a nice color
			double t = static_cast<double>(i) / 
				static_cast<double>(widgets.size());
			glColor3dv(((1.0 - t) * color1 + t * color2).V);
			NQVTK::Vector3 dir = 
				(widgets[i].pos - origin).normalized();
			glVertex3dv((vporigin + widgetRadius * dir).V);
			glVertex3dv((PosToViewport(widgets[i].pos) - 
				widgetRadius * dir).V);
		}
		glEnd();

		// Draw points
		glPointSize(5.0);
		glColor3d(1.0, 1.0, 1.0);
		glEnable(GL_POINT_SMOOTH);
		DrawPoints();

		// Draw widget circles
		glLineWidth(1.5);
		for (unsigned int i = 0; i < widgets.size(); ++i)
		{
			switch (widgets[i].state)
			{
			case PointWidget::Drag:
				glColor3d(1.0, 0.5, 0.0);
				break;
			case PointWidget::Hover:
				glColor3d(1.0, 1.0, 1.0);
				break;
			case PointWidget::Normal:
				glColor3d(0.0, 0.5, 1.0);
			}

			glBegin(GL_LINE_LOOP);
			const int steps = 50;
			NQVTK::Vector3 center = PosToViewport(widgets[i].pos);
			for (int j = 0; j < steps; ++j)
			{
				double t = static_cast<double>(j) 
					/ static_cast<double>(steps) * 2.0 * M_PI;
				NQVTK::Vector3 r(std::cos(t), std::sin(t), 0.0);
				glVertex3dv((center + r * widgetRadius).V);
			}
			glEnd();
		}
		glDisable(GL_BLEND);
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::SetPopulation(Population *population)
	{
		this->population = population;
		// Re-evaluate the number of axes
		SetNumberOfAxes(widgets.size() - 1);
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::SetNumberOfAxes(unsigned int num)
	{
		if (population)
		{
			num = std::min(static_cast<int>(num), 
				population->GetShapeSpaceDimension());
		}
		if (num == widgets.size() - 1) return;
		// Adjust the set of point widgets
		// widgets[0] is the origin
		if (widgets.size() > num + 1)
		{
			widgets.resize(num + 1);
		}
		else
		{
			// TODO: intelligently position new widgets
			PointWidget widgetTemplate = {};
			widgets.resize(num + 1, widgetTemplate);
		}
	}

	// ------------------------------------------------------------------------
	NQVTK::Vector3 ScatterPlotRenderer::ViewportToPos(int x, int y)
	{
		return (NQVTK::Vector3(static_cast<double>(x), 
			static_cast<double>(y), 0.0) - widgetOrigin) / widgetScale;
	}

	// ------------------------------------------------------------------------
	NQVTK::Vector3 ScatterPlotRenderer::PosToViewport(
		const NQVTK::Vector3 &pos)
	{
		return widgetOrigin + widgetScale * pos;
	}

	// ------------------------------------------------------------------------
	PointWidget *ScatterPlotRenderer::WidgetAtPoint(int x, int y)
	{
		NQVTK::Vector3 cp(static_cast<double>(x), static_cast<double>(y), 0.0);
		// Reverse order makes it easier to pick axes instead of the origin
		for (int i = widgets.size() - 1; i >= 0; --i)
		{
			NQVTK::Vector3 wp = PosToViewport(widgets[i].pos);
			if ((wp - cp).length() <= widgetRadius)
			{
				return &widgets[i];
			}
		}
		return 0;
	}

	// ------------------------------------------------------------------------
	itpp::vec ScatterPlotRenderer::GetProjectionXAxis()
	{
		// Should only be called when there is a population
		assert(population);

		itpp::vec result(population->GetShapeSpaceDimension());
		result.zeros();

		// Loop over widgets
		NQVTK::Vector3 origin = widgets[0].pos;
		for (unsigned int i = 1; i < widgets.size(); ++i)
		{
			result(i - 1) = widgets[i].pos.x - origin.x;
		}

		// TODO: Transform vector back to original space
		return result;
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::DrawPoints()
	{
		if (!population) return;

		int numPoints = population->GetNumberOfIndividuals();
		for (int i = 0; i < numPoints; ++i)
		{
			DrawPoint(population->GetIndividual(i));
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::DrawPoint(const itpp::vec &point)
	{
		NQVTK::Vector3 pos = ProjectPoint(point);
		glBegin(GL_POINTS);
		glVertex3dv(PosToViewport(pos * zoom).V);
		glEnd();
	}

	// ------------------------------------------------------------------------
	NQVTK::Vector3 ScatterPlotRenderer::ProjectPoint(const itpp::vec &point)
	{
		NQVTK::Vector3 pos = widgets[0].pos;
		for (unsigned int j = 1; j < widgets.size(); ++j)
		{
			pos += point(j - 1) * (widgets[j].pos - widgets[0].pos);
		}
		return pos;
	}
}
