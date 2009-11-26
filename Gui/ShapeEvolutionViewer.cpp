#include <GLBlaat/GL.h>

#include "ShapeEvolutionViewer.h"
#include "ShapeEvolutionViewer.moc"

#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

#include "Data/LinearPopulationTrajectory.h"

#include "Rendering/ShapeEvolutionRenderer.h"
#include "Rendering/SideBySideConfiguration.h"
#include "Rendering/ShapeStackConfiguration.h"
#include "Rendering/OverlayConfiguration.h"

#include <NQVTK/Interactors/OrbitCameraInteractor.h>

#include <NQVTK/Rendering/OrbitCamera.h>
#include <NQVTK/Rendering/Scene.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeEvolutionViewer::ShapeEvolutionViewer(QWidget *parent) 
		: NQVTKWidget(parent), model(0), trajectory(0)
	{
		// Initialize renderer
		ShapeEvolutionRenderer *renderer = new ShapeEvolutionRenderer();
		SetRenderer(renderer);

		// Create camera and interactor
		NQVTK::OrbitCamera *camera = new NQVTK::OrbitCamera();
		renderer->SetCamera(camera);
		SetInteractor(new NQVTK::OrbitCameraInteractor(camera));

		// TODO: add UI for selecting configuration
		//configuration = new SideBySideConfiguration();
		configuration = new ShapeStackConfiguration();
		//configuration = new OverlayConfiguration();
		renderer->SetConfiguration(configuration);
	}

	// ------------------------------------------------------------------------
	ShapeEvolutionViewer::~ShapeEvolutionViewer()
	{
		delete trajectory;
		delete configuration;
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetShapeModel(ShapeModel *model)
	{
		ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
		assert(renderer != 0);

		// TODO: disconnect signals from old model
		delete trajectory;
		trajectory = 0;

		if (model)
		{
			renderer->SetMesh(model->GetMesh());
			// TODO: add UI for selecting trajectory type
			Population *population = model->GetPopulation();
			if (population)
			{
				trajectory = new LinearPopulationTrajectory(population);
			}
			// TODO: connect signals for new model
		}

		renderer->SetTrajectory(trajectory);

		this->model = model;
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetVector(itpp::vec vector)
	{
		if (trajectory)
		{
			trajectory->SetVector(vector);

			ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
			assert(renderer != 0);
			renderer->UpdateSlices();

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
