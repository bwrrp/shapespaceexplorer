#include <GLBlaat/GL.h>

#include "ScatterPlotViewer.h"
#include "ScatterPlotViewer.moc"

#include "Data/ShapeModel.h"

#include "Rendering/ScatterShapesRenderer.h"

#include "ScatterPlotInteractor.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotViewer::ScatterPlotViewer(QWidget *parent) : NQVTKWidget(parent)
	{
		ScatterPlotRenderer *renderer = new ScatterShapesRenderer();
		SetRenderer(renderer);

		SetInteractor(new ScatterPlotInteractor(renderer));
	}

	// ------------------------------------------------------------------------
	ScatterPlotViewer::~ScatterPlotViewer()
	{
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::SetShapeModel(ShapeModel *model)
	{
		ScatterPlotRenderer *renderer = 
			dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
		assert(renderer != 0);

		// TODO: disconnect signals from the old model
		if (model)
		{
			renderer->SetPopulation(model->GetPopulation());
			// TODO: connect signals for the new model
		}
		else
		{
			renderer->SetPopulation(0);
		}
		this->model = model;
	}
}
