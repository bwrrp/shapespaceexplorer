#include <GLBlaat/GL.h>

#include "MainViewer.h"
#include "MainViewer.moc"

#include <NQVTK/Interactors/ArcballCameraInteractor.h>

#include <NQVTK/Rendering/ArcballCamera.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainViewer::MainViewer(QWidget *parent) : NQVTKWidget(parent)
	{
		// TODO: initialize main viewer
		// renderer = new ...;

		//NQVTK::ArcballCamera *camera = new NQVTK::ArcballCamera();
		//renderer->SetCamera(camera);

		//SetRenderer(renderer);
		//SetInteractor(new NQVTK::ArcballCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	MainViewer::~MainViewer()
	{
	}
}
