#include <GLBlaat/GL.h>

#include "MeshViewer.h"
#include "MeshViewer.moc"

#include "Data/ShapeMesh.h"
#include "Data/ShapeModel.h"

#include <NQVTK/Rendering/Scene.h>
#include <NQVTK/Rendering/SimpleRenderer.h>
#include <NQVTK/Rendering/ArcballCamera.h>
#include <NQVTK/Interactors/ArcballCameraInteractor.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MeshViewer::MeshViewer(QWidget *parent) : NQVTKWidget(parent), model(0)
	{
		// Initialize renderer
		NQVTK::SimpleRenderer *renderer = new NQVTK::SimpleRenderer();
		SetRenderer(renderer);

		// Create a scene with dummy renderable
		scene = new NQVTK::Scene();
		scene->AddRenderable(0);
		renderer->SetScene(scene);

		// Create camera and interactor
		NQVTK::ArcballCamera *camera = new NQVTK::ArcballCamera();
		renderer->SetCamera(camera);
		SetInteractor(new NQVTK::ArcballCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	MeshViewer::~MeshViewer()
	{
		scene->SetRenderable(0, 0);
		delete scene;
	}

	// ------------------------------------------------------------------------
	void MeshViewer::SetShapeModel(ShapeModel *model)
	{
		this->model = model;
		if (model)
		{
			scene->SetRenderable(0, model->GetMesh());
		}
		else
		{
			scene->SetRenderable(0, 0);
		}
		GetRenderer()->SceneChanged();
	}
}
