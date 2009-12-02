#include <GLBlaat/GL.h>

#include "MeshViewer.h"
#include "MeshViewer.moc"

#include "Data/ShapeMesh.h"
#include "Data/ShapeModel.h"

#include "Rendering/MeshRenderer.h"

#include <NQVTK/Rendering/Scene.h>
#include <NQVTK/Rendering/ArcballCamera.h>
#include <NQVTK/Interactors/ArcballCameraInteractor.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MeshViewer::MeshViewer(QWidget *parent) : NQVTKWidget(parent), model(0)
	{
		// Initialize renderer
		MeshRenderer *renderer = new MeshRenderer();
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
		MeshRenderer *renderer = dynamic_cast<MeshRenderer*>(GetRenderer());
		assert(renderer != 0);

		this->model = model;
		if (model)
		{
			scene->SetRenderable(0, model->GetMesh());
			renderer->SetMesh(model->GetMesh());
		}
		else
		{
			scene->SetRenderable(0, 0);
			renderer->SetMesh(0);
		}
		GetRenderer()->SceneChanged();
	}

	// ------------------------------------------------------------------------
	void MeshViewer::SetShape(itpp::vec shape)
	{
		if (!model) return;

		MeshRenderer *renderer = dynamic_cast<MeshRenderer*>(GetRenderer());
		assert(renderer != 0);

		renderer->SetShape(shape);
		updateGL();
	}
}
