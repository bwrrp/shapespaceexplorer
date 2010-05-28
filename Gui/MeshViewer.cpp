#include <GLBlaat/GL.h>

#include "MeshViewer.h"
#include "MeshViewer.moc"

#include "Data/CoordinateFrame.h"
#include "Data/Population.h"
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

		reconstructionDims = 5;
		colorMode = ColorByDeformation;
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
		UpdateReference();
		GetRenderer()->SceneChanged();
	}

	// ------------------------------------------------------------------------
	void MeshViewer::SetColorMode(ColorMode mode)
	{
		this->colorMode = mode;

		UpdateReference();

		updateGL();
	}

	// ------------------------------------------------------------------------
	const itpp::vec MeshViewer::GetShape()
	{
		if (!model) return itpp::vec();

		MeshRenderer *renderer = dynamic_cast<MeshRenderer*>(GetRenderer());
		assert(renderer != 0);

		return renderer->GetShape();
	}

	// ------------------------------------------------------------------------
	void MeshViewer::SetShape(itpp::vec shape)
	{
		if (!model) return;

		MeshRenderer *renderer = dynamic_cast<MeshRenderer*>(GetRenderer());
		assert(renderer != 0);

		renderer->SetShape(shape);
		UpdateReference();
		updateGL();
	}

	// ------------------------------------------------------------------------
	void MeshViewer::SetReconstructionDimension(int dims)
	{
		if (dims == reconstructionDims) return;
		reconstructionDims = dims;
		UpdateReference();
		updateGL();
	}

	// ------------------------------------------------------------------------
	void MeshViewer::UpdateReference()
	{
		MeshRenderer *renderer = dynamic_cast<MeshRenderer*>(GetRenderer());
		assert(renderer != 0);

		// Update the reference shape according to the current coloring mode
		if (!model) return;

		itpp::vec reference(model->GetMesh()->GetShapeSpaceDimension());
		reference.zeros();

		if (colorMode == ColorByReconstructionError)
		{
			Population *population = model->GetPopulation();
			if (population)
			{
				CoordinateFrame *basis = 
					population->GetPrincipalComponentBasis(reconstructionDims);
				reference = basis->TransformOut(
					basis->TransformIn(renderer->GetShape()));
				delete basis;
			}
		}

		renderer->SetReference(reference);
	}
}
