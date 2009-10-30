#include <GLBlaat/GL.h>

#include "MainViewer.h"
#include "MainViewer.moc"

#include "Rendering/ContoursRenderer.h"

#include <NQVTK/Interactors/ArcballCameraInteractor.h>

#include <NQVTK/Rendering/ArcballCamera.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainViewer::MainViewer(QWidget *parent) : NQVTKWidget(parent)
	{
		// Initialize main viewer
		ContoursRenderer *renderer = new ContoursRenderer();

		NQVTK::ArcballCamera *camera = new NQVTK::ArcballCamera();
		renderer->SetCamera(camera);

		SetRenderer(renderer);
		SetInteractor(new NQVTK::ArcballCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	MainViewer::~MainViewer()
	{
	}
}
