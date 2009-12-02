#include <GLBlaat/GL.h>

#include "ShapeEvolutionRenderer.h"

#include "Data/ShapeMesh.h"
#include "Data/Population.h"

#include "Data/Utility.h"

#include "MeshRenderer.h"
#include "EvolutionConfiguration.h"

#include <itpp/itbase.h>

#include <NQVTK/Rendering/Camera.h>
#include <NQVTK/Rendering/Scene.h>

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>
#include <GLBlaat/GLTextureManager.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeEvolutionRenderer::ShapeEvolutionRenderer() 
		: compositeShader(0), meshBuffer(0), configuration(0), trajectory(0)
	{
		meshRenderer = new MeshRenderer();
		// Create the mesh space scene with a dummy renderable
		meshSpace = new NQVTK::Scene();
		meshSpace->AddRenderable(0);
		meshRenderer->SetScene(meshSpace);
		// The gradient background will mess up the G-buffer
		meshRenderer->SetDrawBackground(false);
	}

	// ------------------------------------------------------------------------
	ShapeEvolutionRenderer::~ShapeEvolutionRenderer()
	{
		delete meshRenderer;
		delete compositeShader;
		delete meshBuffer;
		// Don't delete the ShapeMesh!
		meshSpace->SetRenderable(0, 0);
		delete meshSpace;
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionRenderer::Draw()
	{
		// TODO: find out why this doesn't stick...
		meshRenderer->SetScene(meshSpace);

		// Prepare for rendering
		if (fboTarget) fboTarget->Bind();
		glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

		// Clear to 1.0 alpha for front-to-back blending
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Clear();
		// Clear to 0.0 for the G-buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		UpdateLighting();

		// TODO: add support for adding normal renderables to the scene
		//DrawRenderables();

		if (fboTarget) fboTarget->Unbind();

		// TODO: implement fallback configuration (single slice)
		if (configuration)
		{
			// TODO: Determine drawing order
			/*
			double cameraZ = camera->position.z;
			int numSlices = stack->GetNumberOfSlices();
			itpp::vec relSliceOffsets(numSlices);
			for (int i = 0; i < numSlices; ++i)
			{
				relSliceOffsets(i) = abs(cameraZ - stack->GetSliceOffset(i));
			}
			itpp::ivec order = itpp::sort_index(relSliceOffsets);
			*/

			// Set up camera
			camera->focus = NQVTK::Vector3();
			camera->SetZPlanes(configuration->GetBounds());

			// Now render the current set of slices
			for (unsigned int i = 0; i < slices.size(); ++i)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// Render this slice
				const EvolutionSlice &slice = slices[i];
				meshRenderer->SetShape(slice.shape);
				meshRenderer->Draw();

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// Composite into our target
				if (fboTarget) fboTarget->Bind();
				// Use front-to-back blending
				glBlendFuncSeparate(
					GL_DST_ALPHA, GL_ONE, 
					GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);

				camera->Draw();

				compositeShader->Start();
				tm->SetupProgram(compositeShader);
				tm->Bind();

				// Draw the slice
				compositeShader->SetUniform1f("offset", 
					static_cast<float>(slices[i].offset));
				compositeShader->SetUniform1f("contourColoring", 
					static_cast<float>(slices[i].contourColoring));
				compositeShader->SetUniform1f("contourOpacity", 
					static_cast<float>(slices[i].contourOpacity));
				compositeShader->SetUniform1f("shapeOpacity", 
					static_cast<float>(slices[i].shapeOpacity));
				slices[i].Draw();

				compositeShader->Stop();
				tm->Unbind();

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				if (fboTarget) fboTarget->Unbind();
			}
		}

		// Blend in the background last
		if (fboTarget) fboTarget->Bind();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBegin(GL_QUADS);
		glColor4d(0.2, 0.2, 0.25, 0.0);
		glVertex3d(-1.0, -1.0, 0.0);
		glVertex3d(1.0, -1.0, 0.0);
		glColor4d(0.6, 0.6, 0.65, 0.0);
		glVertex3d(1.0, 1.0, 0.0);
		glVertex3d(-1.0, 1.0, 0.0);
		glEnd();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		if (fboTarget) fboTarget->Unbind();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionRenderer::SetMesh(ShapeMesh *mesh)
	{
		meshSpace->SetRenderable(0, mesh);
		meshRenderer->SetMesh(mesh);
		meshRenderer->SceneChanged();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionRenderer::SetTrajectory(ShapeTrajectory *trajectory)
	{
		this->trajectory = trajectory;
		UpdateSlices();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionRenderer::SetConfiguration(
		EvolutionConfiguration *configuration)
	{
		this->configuration = configuration;
		UpdateSlices();
	}

	// ------------------------------------------------------------------------
	void ShapeEvolutionRenderer::UpdateSlices()
	{
		if (configuration != 0 && trajectory != 0)
		{
			configuration->SetupSlices(slices, trajectory);
		}
		else
		{
			slices.clear();
		}
	}

	// ------------------------------------------------------------------------
	NQVTK::Camera *ShapeEvolutionRenderer::GetMeshCamera()
	{
		if (!meshRenderer) return 0;
		return meshRenderer->GetCamera();
	}

	// ------------------------------------------------------------------------
	bool ShapeEvolutionRenderer::Initialize()
	{
		if (!Superclass::Initialize()) return false;

		if (!meshRenderer->TryInitialize()) return false;

		bool ok;

		// Set up shader for compositing
		delete compositeShader;
		compositeShader = GLProgram::New();
		ok = compositeShader != 0;
		if (ok) ok = compositeShader->AddVertexShader(
			Utility::LoadShader("CompositeShaderVS.txt"));
		if (ok) ok = compositeShader->AddFragmentShader(
			Utility::LoadShader("CompositeShaderFS.txt"));
		if (ok) ok = compositeShader->Link();
		if (!ok)
		{
			std::cerr << "Error creating composite shader" << std::endl;
			delete compositeShader;
			compositeShader = 0;
		}

		// Create framebuffer for mesh rendering
		// TODO: make this size adaptive to viewport and projection?
		meshBuffer = GLFramebuffer::New(1024, 1024);
		ok = meshBuffer != 0;
		if (ok)
		{
			meshBuffer->CreateDepthBuffer();
			meshBuffer->CreateColorTexture();
			GLenum bufs[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, bufs);
			ok = meshBuffer->IsOk();
			meshBuffer->Unbind();
		}
		if (!ok)
		{
			std::cerr << "Error creating buffer for mesh rendering" << 
				std::endl;
			delete meshBuffer;
			meshBuffer = 0;
		}
		meshRenderer->SetTarget(meshBuffer);
		meshRenderer->SetViewport(0, 0, 1024, 1024);
		tm->AddTexture("meshBuffer", 
			meshBuffer->GetTexture2D(GL_COLOR_ATTACHMENT0), false);

		return compositeShader != 0 &&
			meshBuffer != 0;
	}
}
