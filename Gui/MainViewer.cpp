#include <GLBlaat/GL.h>

#include "MainViewer.h"
#include "MainViewer.moc"

#include "Rendering/ShapeStackRenderer.h"

#include <NQVTK/Interactors/ArcballCameraInteractor.h>

#include <NQVTK/Rendering/ArcballCamera.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainViewer::MainViewer(QWidget *parent) : NQVTKWidget(parent)
	{
		// Initialize main viewer
		ShapeStackRenderer *renderer = new ShapeStackRenderer();
		SetRenderer(renderer);

		NQVTK::ArcballCamera *camera = new NQVTK::ArcballCamera();
		renderer->SetCamera(camera);

		SetInteractor(new NQVTK::ArcballCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	MainViewer::~MainViewer()
	{
	}

	// ------------------------------------------------------------------------
	void MainViewer::SetShapeStack(ShapeStack *stack)
	{
		ShapeStackRenderer *renderer = 
			dynamic_cast<ShapeStackRenderer*>(GetRenderer());
		assert(renderer != 0);
		renderer->SetShapeStack(stack);
	}
}
