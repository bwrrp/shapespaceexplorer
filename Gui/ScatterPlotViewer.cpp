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
	ScatterPlotViewer::ScatterPlotViewer(QWidget *parent) 
		: NQVTKWidget(parent), model(0)
	{
		ScatterPlotRenderer *renderer = new ScatterShapesRenderer();
		SetRenderer(renderer);

		ScatterPlotInteractor *interactor = 
			new ScatterPlotInteractor(renderer);
		SetInteractor(interactor);
		connect(interactor->GetMessenger(), SIGNAL(ProjectionChanged()), 
			this, SLOT(OnProjectionChanged()));
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
		this->model = model;
		if (model)
		{
			renderer->SetPopulation(model->GetPopulation());
			// TODO: maybe just pass the model instead?
			ScatterShapesRenderer *shapesRenderer = 
				dynamic_cast<ScatterShapesRenderer*>(renderer);
			if (shapesRenderer) shapesRenderer->SetMesh(model->GetMesh());

			// TODO: connect signals for the new model
			OnProjectionChanged();
		}
		else
		{
			renderer->SetPopulation(0);
			ScatterShapesRenderer *shapesRenderer = 
				dynamic_cast<ScatterShapesRenderer*>(renderer);
			if (shapesRenderer) shapesRenderer->SetMesh(0);
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::OnProjectionChanged()
	{
		if (!model) return;

		// Ignore changes if no population has been loaded
		if (model->GetPopulation() != 0)
		{
			ScatterPlotRenderer *renderer = 
				dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
			assert(renderer != 0);

			emit XAxisChanged(renderer->GetProjectionXAxis());
		}
	}
}
