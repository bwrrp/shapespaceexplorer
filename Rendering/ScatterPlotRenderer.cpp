#define _USE_MATH_DEFINES

#include <GLBlaat/GL.h>

#include "ScatterPlotRenderer.h"

#include "Data/CoordinateFrame.h"
#include "Data/Population.h"
#include "Data/PickInfo.h"

#include "GPUVoronoi.h"

#include <cassert>
#include <cmath>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotRenderer::ScatterPlotRenderer() 
		: population(0), frame(0), populationInFrame(0)
	{
		widgetRadius = 6.0;
		zoom = 1.0;
		// Add the origin widget
		PointWidget origin = {};
		widgets.push_back(origin);
		// Start with four axes
		SetNumberOfAxes(5);

		//pickInfo = new PickInfo();
		voronoi = new GPUVoronoi();

		lastWidget = 0;
	}

	// ------------------------------------------------------------------------
	ScatterPlotRenderer::~ScatterPlotRenderer()
	{
		//delete pickInfo;
		delete voronoi;
		delete frame;
		delete populationInFrame;
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

		// Update the Voronoi buffer
		voronoi->Resize(w, h);
		UpdatePickInfo();
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

		glEnable(GL_LINE_SMOOTH);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		// Draw a visualization of the shape model distribution
		DrawDistribution();

		// Draw axes
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
		// Reset the coordinate frame to identity and recompute cache
		SetFrame(0);
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::SetFrame(CoordinateFrame *frame)
	{
		if (frame != 0 && frame == this->frame) return;

		delete this->frame;

		// If no frame is given, use the default (identity)
		if (population != 0 && frame == 0)
		{
			int numAxes = static_cast<int>(requestedNumAxes);
			int spaceDim = population->GetShapeSpaceDimension();
			numAxes = std::min(numAxes, spaceDim);
			frame = CoordinateFrame::PartialIdentityBasis(spaceDim, numAxes);
		}

		this->frame = frame;

		// Update cached population
		delete populationInFrame;
		if (population)
		{
			populationInFrame = population->TransformTo(frame);
		}
		else
		{
			populationInFrame = 0;
		}

		// Re-evaluate the number of axes
		SetNumberOfAxes(requestedNumAxes);

		ZoomToFit();
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::SetNumberOfAxes(unsigned int num)
	{
		requestedNumAxes = num;

		if (populationInFrame)
		{
			num = std::min(static_cast<int>(num), 
				populationInFrame->GetShapeSpaceDimension());
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
			PointWidget widgetTemplate = {};
			unsigned int first = widgets.size();
			assert(first > 0);
			widgets.resize(num + 1, widgetTemplate);
			if (first <= 5)
			{
				// Position first 5 widgets on a circle
				for (unsigned int i = first; i < num + 1; ++i)
				{
					double t = static_cast<double>(i - 1) / 5.0 * 2.0 * M_PI;
					widgets[i].pos.x = std::cos(t);
					widgets[i].pos.y = std::sin(t);
				}
			}
			else
			{
				// Nicely interpolate between the last and first widget
				double delta = 1.0 / static_cast<double>(num + 2 - first);
				NQVTK::Vector3 v1 = widgets[1].pos - widgets[0].pos;
				NQVTK::Vector3 v2 = widgets[first - 1].pos - widgets[0].pos;
				double r1 = v1.length();
				double r2 = v2.length();
				for (unsigned int i = first; i < num + 1; ++i)
				{
					double d = static_cast<double>(i - first + 1) * delta;
					NQVTK::Vector3 offset = (1.0 - d) * v1 + d * v2;
					double r = (1.0 - d) * r1 + d * r2;
					widgets[i].pos = widgets[0].pos + r * offset.normalized();
				}
			}
		}
		lastWidget = 0;
	}

	// ------------------------------------------------------------------------
	unsigned int ScatterPlotRenderer::GetNumberOfAxes()
	{
		return widgets.size() - 1;
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
				lastWidget = i;
				return &widgets[i];
			}
		}
		return 0;
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::UpdatePickInfo()
	{
		if (!populationInFrame) return;

		std::vector<NQVTK::Vector3> points;
		int numPoints = populationInFrame->GetNumberOfIndividuals();
		for (int i = 0; i < numPoints; ++i)
		{
			//points.push_back(ProjectPoint(
			//	populationInFrame->GetIndividual(i)));
			points.push_back(PosToViewport(zoom * 
				ProjectPoint(populationInFrame->GetIndividual(i))));
		}
		//pickInfo->UpdateInfo(points);
		voronoi->UpdateVoronoi(points);
	}

	// ------------------------------------------------------------------------
	itpp::vec ScatterPlotRenderer::PickShape(int x, int y)
	{
		// Should only be called when there is a population
		// TODO: choose between original and reconstructed shape
		assert(population != 0);

		NQVTK::Vector3 pos(static_cast<double>(x), 
			static_cast<double>(y), 0.0);

		// Get weights
		std::vector<double> weights = voronoi->QueryPoint(pos);

		// Interpolate shape vectors
		itpp::vec result(population->GetShapeSpaceDimension());
		result.zeros();
		for (unsigned int i = 0; i < weights.size(); ++i)
		{
			result += weights[i] * population->GetIndividual(i);
		}

		/*
		NQVTK::Vector3 pos = ViewportToPos(x, y);
		pos = pos / zoom;

		// Find cell containing point
		int id1, id2, id3;
		double u, v;
		bool inHull = pickInfo->Pick(pos, id1, id2, id3, u, v);

		// Interpolate shape vectors
		itpp::vec result(population->GetShapeSpaceDimension());
		result.zeros();
		if (inHull)
		{
			double w = 1.0 - u - v;
			itpp::vec a = population->GetIndividual(id1);
			itpp::vec b = population->GetIndividual(id2);
			itpp::vec c = population->GetIndividual(id3);
			result = w * a + v * b + u * c;
		}
		else
		{
			// TODO: perform extrapolation when pointing outside the hull
		}
		*/

		return result;
	}

	// ------------------------------------------------------------------------
	itpp::vec ScatterPlotRenderer::GetProjectionXAxis()
	{
		// Should only be called when there is a population
		assert(populationInFrame);

		itpp::vec result(populationInFrame->GetShapeSpaceDimension());
		result.zeros();

		// Loop over widgets
		NQVTK::Vector3 origin = widgets[0].pos;
		for (unsigned int i = 1; i < widgets.size(); ++i)
		{
			result(i - 1) = widgets[i].pos.x - origin.x;
		}

		// Transform vector back to original space
		return frame->TransformOut(result);
	}

	// ------------------------------------------------------------------------
	itpp::vec ScatterPlotRenderer::GetProjectionYAxis()
	{
		// Should only be called when there is a population
		assert(population);

		itpp::vec result(populationInFrame->GetShapeSpaceDimension());
		result.zeros();

		// Loop over widgets
		NQVTK::Vector3 origin = widgets[0].pos;
		for (unsigned int i = 1; i < widgets.size(); ++i)
		{
			result(i - 1) = widgets[i].pos.y - origin.y;
		}

		// Transform vector back to original space
		return frame->TransformOut(result);
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::ZoomToFit()
	{
		if (!populationInFrame) return;

		int numPoints = populationInFrame->GetNumberOfIndividuals();

		// Compute extents for current projection
		double max = 0.0;
		for (int i = 0; i < numPoints; ++i)
		{
			itpp::vec point = populationInFrame->GetIndividual(i);
			NQVTK::Vector3 pos = ProjectPoint(point);
			max = std::max(max, std::max(std::abs(pos.x), std::abs(pos.y)));
		}
		zoom = 1.0 / max;

		// Update picking info
		UpdatePickInfo();
	}

	// ------------------------------------------------------------------------
	bool ScatterPlotRenderer::Initialize()
	{
		if (!Superclass::Initialize()) return false;

		return voronoi->Initialize();
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::DrawDistribution()
	{
		if (!population) return;

		int numComponents = widgets.size() - 1;
		// Construct projection matrix
		itpp::mat projection(2, numComponents);
		projection.zeros();
		NQVTK::Vector3 &origin = widgets[0].pos;
		for (int i = 0; i < numComponents; ++i)
		{
			itpp::vec axis((widgets[i + 1].pos - origin).V, 2);
			projection.set_col(i, axis);
		}
		// Get covariance matrix
		itpp::mat covariance = populationInFrame->GetCovariance(numComponents);
		// Project the covariance matrix
		itpp::mat projCovariance = 
			projection * covariance * projection.transpose();
		// Perform eigenanalysis to find 2D Gaussian parameters
		itpp::vec eigVals;
		itpp::mat eigVecs;
		itpp::eig_sym(projCovariance, eigVals, eigVecs);
		itpp::ivec index = itpp::sort_index(eigVals);
		index = itpp::reverse(index);
		// Draw ellipses at 1, 2, 3 * stdev
		NQVTK::Vector3 v1(eigVecs(0, index(0)), eigVecs(1, index(0)), 0.0);
		NQVTK::Vector3 v2(eigVecs(0, index(1)), eigVecs(1, index(1)), 0.0);
		v1 = v1.normalized() * sqrt(eigVals(index(0)));
		v2 = v2.normalized() * sqrt(eigVals(index(1)));
		NQVTK::Vector3 color1(1.0, 0.1, 0.1);
		NQVTK::Vector3 color2(0.5, 0.1, 0.1);
		for (int n = 0; n < 3; ++n)
		{
			double r = static_cast<double>(n + 1);
			double t = static_cast<double>(n) / 2.0;
			glBegin(GL_LINE_LOOP);
			glColor3dv((color1 * (1.0 - t) + color2 * t).V);
			for (int i = 0; i < 100; ++i)
			{
				double a = static_cast<double>(i) / 100.0 * 2.0 * M_PI;
				glVertex3dv(PosToViewport((origin + 
					sin(a) * r * v1 + cos(a) * r * v2) * zoom).V);
			}
			glEnd();
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::DrawPoints()
	{
		if (!populationInFrame) return;

		int numPoints = populationInFrame->GetNumberOfIndividuals();

		// Determine drawing order
		itpp::vec axis(populationInFrame->GetShapeSpaceDimension());
		axis.zeros();
		if (lastWidget > 0)
		{
			// Sort by the last widget picked
			axis(lastWidget - 1) = 1.0;
		}
		else
		{
			// If the origin is dragged, sort by the combination of all vectors
			// TODO: take weights into account here?
			int numAxes = widgets.size() - 1;
			if (numAxes == 0) axis(0) = 1.0;
			double v = -sqrt(1.0 / static_cast<double>(numAxes));
			for (int i = 0; i < numAxes; ++i)
			{
				axis(i) = v;
			}
		}
		itpp::vec projections(numPoints);
		for (int i = 0; i < numPoints; ++i)
		{
			projections(i) = itpp::dot(axis, 
				populationInFrame->GetIndividual(i));
		}
		itpp::ivec order = itpp::sort_index(projections);

		// Determine reconstruction errors for all individuals
		itpp::vec error(numPoints);
		itpp::vec prob(numPoints);
		for (int i = 0; i < numPoints; ++i)
		{
			error(i) = population->GetReconstructionError(
				population->GetIndividual(i), GetNumberOfAxes());
			prob(i) = population->GetProbability(
				population->GetIndividual(i), GetNumberOfAxes());
		}
		double maxProb = itpp::max(prob);

		for (int i = 0; i < numPoints; ++i)
		{
			// Color by reconstruction error
			// TODO: make this configurable
			// TODO: make the color map perceptually uniform
			NQVTK::Vector3 colorLow(0.0, 0.5, 1.0);
			NQVTK::Vector3 colorHigh(1.0, 0.5, 0.0);
			//double p = error(order(i));
			double p = 1.0 - prob(order(i)) / maxProb;
			// TODO: make the color-mapped interval configurable
			//p = -1.0 + 0.02 * p;
			if (p < 0.0) p = 0.0;
			if (p > 1.0) p = 1.0;
			glColor3dv(((1.0 - p) * colorLow + p * colorHigh).V);
			DrawPoint(order(i));
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotRenderer::DrawPoint(int individual)
	{
		itpp::vec point = populationInFrame->GetIndividual(individual);
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
