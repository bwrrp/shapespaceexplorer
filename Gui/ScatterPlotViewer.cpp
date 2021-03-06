#include <GLBlaat/GL.h>

#include "ScatterPlotViewer.h"
#include "ScatterPlotViewer.moc"

#include "Data/Population.h"
#include "Data/ShapeModel.h"

#include "Rendering/ScatterShapesRenderer.h"

#include "ScatterPlotInteractor.h"

#include <NQVTK/Rendering/Camera.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterPlotViewer::ScatterPlotViewer(QWidget *parent) 
		: NQVTKWidget(parent), model(0), usePCAFrame(false)
	{
		ScatterPlotRenderer *renderer = new ScatterShapesRenderer();
		SetRenderer(renderer);

		ScatterPlotInteractor *interactor = 
			new ScatterPlotInteractor(renderer);
		SetInteractor(interactor);
		connect(interactor->GetMessenger(), SIGNAL(CursorPosChanged(int, int)), 
			this, SLOT(OnCursorPosChanged(int, int)));
		connect(interactor->GetMessenger(), SIGNAL(ProjectionChanged()), 
			this, SLOT(OnProjectionChanged()));

		// To allow receiving keyboard events
		setFocusPolicy(Qt::ClickFocus);
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
			Population *population = model->GetPopulation();
			renderer->SetPopulation(population);
			if (usePCAFrame && population != 0)
			{
				renderer->SetFrame(population->GetPrincipalComponentBasis());
			}
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
	void ScatterPlotViewer::SetColorMode(ScatterPlotRenderer::ColorMode mode)
	{
		ScatterPlotRenderer *renderer = 
			dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
		assert(renderer != 0);
		renderer->SetColorMode(mode);
		updateGL();
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::SyncMeshCamera(NQVTK::Camera *cam)
	{
		ScatterShapesRenderer *renderer = 
			dynamic_cast<ScatterShapesRenderer*>(GetRenderer());
		if (!renderer) return;

		NQVTK::Camera *meshCam = renderer->GetMeshCamera();
		if (meshCam)
		{
			meshCam->position = cam->position;
			meshCam->focus = cam->focus;
			meshCam->up = cam->up;
			updateGL();
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::SetUsePCAFrame(bool use)
	{
		if (use == usePCAFrame) return;
		usePCAFrame = use;

		Population *population = model->GetPopulation();
		if (!population) return;

		ScatterPlotRenderer *renderer = 
			dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
		assert(renderer != 0);

		if (use)
		{
			renderer->SetFrame(population->GetPrincipalComponentBasis());
		}
		else
		{
			renderer->SetFrame(0);
		}
		OnProjectionChanged();
		updateGL();
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::keyPressEvent(QKeyEvent *event)
	{
		ScatterPlotRenderer *renderer = 
			dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
		assert(renderer != 0);

		ScatterPlotInteractor *interactor = 
			dynamic_cast<ScatterPlotInteractor*>(GetInteractor());

		if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Minus)
		{
			int numAxes = renderer->GetNumberOfAxes();
			if (event->key() == Qt::Key_Plus) 
			{
				numAxes += 1;
			}
			else
			{
				numAxes -= 1;
			}
			if (numAxes > 0)
			{
				// Widgets might move in memory, so prevent dangling pointers
				interactor->ReleaseWidgets();
				renderer->SetNumberOfAxes(numAxes);
				OnProjectionChanged();
				emit NumberOfAxesChanged(renderer->GetNumberOfAxes());
			}

			event->accept();
			// Recreate the default frame if necessary
			if (!usePCAFrame) renderer->SetFrame(0);
			updateGL();
		}
		else
		{
			event->ignore();
		}
	}

	// ------------------------------------------------------------------------
	void ScatterPlotViewer::OnCursorPosChanged(int x, int y)
	{
		if (!model) return;

		// Ignore changes if no population has been loaded
		if (model->GetPopulation() != 0)
		{
			ScatterPlotRenderer *renderer = 
				dynamic_cast<ScatterPlotRenderer*>(GetRenderer());
			assert(renderer != 0);

			makeCurrent();
			emit PointSelected(renderer->PickShape(x, y));
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

			// Recompute the picking structure
			makeCurrent();
			renderer->UpdatePickInfo();

			emit XAxisChanged(renderer->GetProjectionXAxis());
		}
	}
}
