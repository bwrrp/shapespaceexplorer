#pragma once

#include <NQVTK/Rendering/Renderer.h>

#include <NQVTK/Math/Vector3.h>

#include <itpp/itbase.h>

#include <vector>

namespace Diverse
{
	struct PointWidget
	{
		NQVTK::Vector3 pos;
		enum {
			Normal = 0, 
			Hover = 1, 
			Drag = 2
		} state;
	};

	class Population;

	class ScatterPlotRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ScatterPlotRenderer();
		virtual ~ScatterPlotRenderer();

		virtual void SetViewport(int x, int y, int w, int h);

		virtual void Draw();

		void SetPopulation(Population *population);
		void SetNumberOfAxes(unsigned int num);

		NQVTK::Vector3 ViewportToPos(int x, int y);
		NQVTK::Vector3 PosToViewport(const NQVTK::Vector3 &pos);

		PointWidget *WidgetAtPoint(int x, int y);

		itpp::vec GetProjectionXAxis();

		double zoom;

	protected:
		virtual void DrawPoints();
		virtual void DrawPoint(const itpp::vec &point);

		NQVTK::Vector3 ProjectPoint(const itpp::vec &point);

		Population *population;

		std::vector<PointWidget> widgets;
		double widgetRadius;
		double widgetScale;
		NQVTK::Vector3 widgetOrigin;
	};
}
