#include <GLBlaat/GL.h>

#include "ShapeEvolutionViewer.h"
#include "ShapeEvolutionViewer.moc"

#include "Data/Population.h"
#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

#include "Data/LinearPopulationTrajectory.h"

#include "Rendering/ShapeEvolutionRenderer.h"
#include "Rendering/SideBySideConfiguration.h"

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

		configuration = new SideBySideConfiguration();
		//configuration = new ShapeStackConfiguration();
		//configuration = new OverlayConfiguration();
		renderer->SetConfiguration(configuration);

		reconstructionDims = 5;
		colorMode = ColorByDeformation;
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

		UpdateReference();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetConfiguration(EvolutionConfiguration *config)
	{
		ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
		assert(renderer != 0);

		// TODO: smoothly morph to the new configuration
		delete this->configuration;
		this->configuration = config;
		renderer->SetConfiguration(config);
		updateGL();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetColorMode(ColorMode mode)
	{
		this->colorMode = mode;

		UpdateReference();

		updateGL();
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

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::SetReconstructionDimension(int dims)
	{
		if (reconstructionDims == dims) return;
		reconstructionDims = dims;

		UpdateReference();

		updateGL();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionViewer::UpdateReference()
	{
		if (!model) return;

		ShapeEvolutionRenderer *renderer = 
			dynamic_cast<ShapeEvolutionRenderer*>(GetRenderer());
		assert(renderer != 0);

		Population *population = model->GetPopulation();
		if (population != 0 && colorMode == ColorByReconstructionError)
		{
			renderer->SetReferenceBasis(
				population->GetPrincipalComponentBasis(reconstructionDims));
		}
		else
		{
			renderer->SetReferenceBasis(0);
		}
	}
}
