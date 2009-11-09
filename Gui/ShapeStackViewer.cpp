#include <GLBlaat/GL.h>

#include "ShapeStackViewer.h"
#include "ShapeStackViewer.moc"

#include "Rendering/ShapeStackRenderer.h"

#include <NQVTK/Interactors/ArcballCameraInteractor.h>

#include <NQVTK/Rendering/ArcballCamera.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeStackViewer::ShapeStackViewer(QWidget *parent) : NQVTKWidget(parent)
	{
		// Initialize main viewer
		ShapeStackRenderer *renderer = new ShapeStackRenderer();
		SetRenderer(renderer);

		NQVTK::ArcballCamera *camera = new NQVTK::ArcballCamera();
		renderer->SetCamera(camera);

		SetInteractor(new NQVTK::ArcballCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	ShapeStackViewer::~ShapeStackViewer()
	{
	}

	// ------------------------------------------------------------------------
	void ShapeStackViewer::SetShapeStack(ShapeStack *stack)
	{
		ShapeStackRenderer *renderer = 
			dynamic_cast<ShapeStackRenderer*>(GetRenderer());
		assert(renderer != 0);
		renderer->SetShapeStack(stack);
	}
}
