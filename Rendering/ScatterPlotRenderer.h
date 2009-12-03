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

	class GPUVoronoi;
	class PickInfo;
	class Population;
	class CoordinateFrame;

	class ScatterPlotRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ScatterPlotRenderer();
		virtual ~ScatterPlotRenderer();

		virtual void SetViewport(int x, int y, int w, int h);

		virtual void Draw();

		void SetPopulation(Population *population);
		void SetFrame(CoordinateFrame *frame);
		void SetNumberOfAxes(unsigned int num);
		unsigned int GetNumberOfAxes();

		NQVTK::Vector3 ViewportToPos(int x, int y);
		NQVTK::Vector3 PosToViewport(const NQVTK::Vector3 &pos);

		PointWidget *WidgetAtPoint(int x, int y);

		void UpdatePickInfo();
		itpp::vec PickShape(int x, int y);

		itpp::vec GetProjectionXAxis();
		itpp::vec GetProjectionYAxis();

		enum ColorMode
		{
			ColoringOff,
			ColorByProbability, 
			ColorByReconstructionError
		};

		void SetColorMode(ColorMode mode);

		virtual void ZoomToFit();

		double zoom;

	protected:
		virtual bool Initialize();

		void DrawDistribution();

		virtual void DrawPoints();
		virtual void DrawPoint(int individual);

		NQVTK::Vector3 ProjectPoint(const itpp::vec &point);

		PickInfo *pickInfo;
		GPUVoronoi *voronoi;

		Population *population;
		CoordinateFrame *frame;
		Population *populationInFrame;

		std::vector<PointWidget> widgets;
		double widgetRadius;
		double widgetScale;
		NQVTK::Vector3 widgetOrigin;
		int lastWidget;
		unsigned int requestedNumAxes;

		ColorMode colorMode;
	};
}
