#include <GLBlaat/GL.h>

#include "ShapeEvolutionViewer.h"
#include "ShapeEvolutionViewer.moc"

#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

#include "Rendering/ShapeStack.h"
#include "Rendering/ShapeEvolutionRenderer.h"
#include "Rendering/PopulationProjectionStack.h"

#include <NQVTK/Interactors/OrbitCameraInteractor.h>

#include <NQVTK/Rendering/OrbitCamera.h>
#include <NQVTK/Rendering/Scene.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeEvolutionViewer::ShapeEvolutionViewer(QWidget *parent) 
		: NQVTKWidget(parent), model(0), stack(0)
	{
		// Initialize renderer
		ShapeEvolutionRenderer *renderer = new ShapeEvolutionRenderer();
		SetRenderer(renderer);

		// Create a scene with dummy renderable
		scene = new NQVTK::Scene();
		scene->AddRenderable(0);

		// Create camera and interactor
		NQVTK::OrbitCamera *camera = new NQVTK::OrbitCamera();
		renderer->SetCamera(camera);
		SetInteractor(new NQVTK::OrbitCameraInteractor(camera));
	}

	// ------------------------------------------------------------------------
	ShapeEvolutionViewer::~ShapeEvolutionViewer()
	{
		// The mesh is owned by the model
		scene->SetRenderable(0, 0);
		delete scene;
		delete stack;
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetShapeModel(ShapeModel *model)
	{
		ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
		assert(renderer != 0);

		// TODO: disconnect signals from old model
		delete stack;

		if (model)
		{
			scene->SetRenderable(0, model->GetMesh());
			// TODO: add UI for selecting the stack type
			stack = new PopulationProjectionStack(model);
			// TODO: connect signals for new model
		}
		else
		{
			scene->SetRenderable(0, 0);
			stack = 0;
		}
		renderer->SetShapeStack(stack);

		this->model = model;
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetVector(itpp::vec vector)
	{
		if (stack)
		{
			stack->SetVector(vector);
			updateGL();
		}
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SyncMeshCamera(NQVTK::Camera *cam)
	{
		ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
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
}
