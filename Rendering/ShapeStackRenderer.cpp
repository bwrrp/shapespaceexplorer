#include <GLBlaat/GL.h>

#include "ShapeStackRenderer.h"

#include "Data/ShapeMesh.h"
#include "Data/Population.h"

#include "Rendering/ShapeStack.h"

#include <itpp/itbase.h>

#include <NQVTK/Rendering/Camera.h>
#include <NQVTK/Rendering/Scene.h>
#include <NQVTK/Rendering/SimpleRenderer.h>

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>
#include <GLBlaat/GLTextureManager.h>

#include <QApplication>
#include <QTime>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace Diverse
{
	// ------------------------------------------------------------------------
	// Helper function
	static std::string LoadShader(const std::string &filename)
	{
		// TODO: embed shader sources in the app for final version
		std::string apppath = QApplication::applicationDirPath().toStdString();
		// Check up to 4 levels up in the directory hierarchy
		std::string searchpath = "/Shaders/" + filename;
		std::ifstream infile((apppath + searchpath).c_str());
		for (int i = 0; i < 4 && !infile; ++i)
		{
			searchpath =  "/.." + searchpath;
			infile.close();
			infile.clear();
			infile.open((apppath + searchpath).c_str());
		}
		std::ostringstream contents;
		if (infile.good())
		{
			contents << infile.rdbuf();
		}
		else
		{
			std::cerr << "Error loading shader!" << std::endl;
		}
		return contents.str();
	}

	// ------------------------------------------------------------------------
	ShapeStackRenderer::ShapeStackRenderer() 
		: NQVTK::NestedRenderer(new NQVTK::SimpleRenderer()), 
		stack(0), compositeShader(0), meshBuffer(0)
	{
		// Create the mesh space scene with a dummy renderable
		meshSpace = new NQVTK::Scene();
		meshSpace->AddRenderable(0);
		baseRenderer->SetScene(meshSpace);
		// The gradient background will mess up the G-buffer
		NQVTK::SimpleRenderer *renderer = 
			dynamic_cast<NQVTK::SimpleRenderer*>(baseRenderer);
		renderer->SetDrawBackground(false);
	}

	// ------------------------------------------------------------------------
	ShapeStackRenderer::~ShapeStackRenderer()
	{
		delete compositeShader;
		delete meshBuffer;
		// This will also delete the ShapeMesh
		delete meshSpace;
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::Clear()
	{
		Superclass::Clear();
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::Draw()
	{
		// TODO: find out why this doesn't stick...
		baseRenderer->SetScene(meshSpace);

		// Prepare for rendering
		if (fboTarget) fboTarget->Bind();
		glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

		// Clear to 1.0 alpha for front-to-back blending
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		Clear();
		// Clear to 0.0 for the G-buffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		UpdateLighting();

		// TODO: fix the propagation mess in the NQVTK::NestedRenderer
		//DrawCamera();
		// TODO: add support for adding normal renderables to the scene
		//DrawRenderables();

		if (fboTarget) fboTarget->Unbind();

		// Now render the shape stack...
		if (stack)
		{
			// Determine drawing order
			double cameraZ = camera->position.z;
			int numSlices = stack->GetNumberOfSlices();
			itpp::vec relSliceOffsets(numSlices);
			for (int i = 0; i < numSlices; ++i)
			{
				relSliceOffsets(i) = abs(cameraZ - stack->GetSliceOffset(i));
			}
			itpp::ivec order = itpp::sort_index(relSliceOffsets);

			// Render slices
			ShapeMesh *mesh = stack->GetMesh();
			for (int i = 0; i < numSlices; ++i)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// Render this slice
				int slice = order(i);
				stack->SetupSliceMesh(slice);
				baseRenderer->Draw();

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

				// TODO: determine slice size automatically
				double bounds[] = {-10.0, 10.0, -10.0, 10.0, -10.0, 10.0};
				camera->focus = NQVTK::Vector3();
				camera->SetZPlanes(bounds);
				camera->Draw();

				compositeShader->Start();
				tm->SetupProgram(compositeShader);
				tm->Bind();

				// Draw the slice
				double offset = stack->GetSliceOffset(slice);
				glBegin(GL_QUADS);
				glTexCoord2d(0.0, 1.0);
				glVertex3d(-10.0, -10.0, offset);
				glTexCoord2d(1.0, 1.0);
				glVertex3d(10.0, -10.0, offset);
				glTexCoord2d(1.0, 0.0);
				glVertex3d(10.0, 10.0, offset);
				glTexCoord2d(0.0, 0.0);
				glVertex3d(-10.0, 10.0, offset);
				glEnd();

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

		// Old code: some testing
		/*
		static int irk = 0;
		if (mesh != 0 && pop != 0)
		{
			//mesh->SetShape(pop->GetIndividual(irk));
			//irk = (irk + 1) % pop->GetNumberOfIndividuals();
			if (pop->GetNumberOfPrincipalComponents() > 0)
			{
				// Animate through the principal components
				QTime now = QTime::currentTime();
				int time = now.msec() + now.second() * 1000 + 
					now.minute() * 60000 + now.hour() * 3600000;
				irk = (time / 5000) % 
					pop->GetNumberOfPrincipalComponents();
				double delta = static_cast<double>(time % 5000) / 
					5000.0 * 6.0 - 3.0;
				mesh->SetShape(delta * pop->GetPrincipalComponent(irk));
			}
		}
		*/
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::SceneChanged()
	{
		// Overridden to prevent propagation to the baseRenderer
		Renderer::SceneChanged();
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::SetShapeStack(ShapeStack *stack)
	{
		this->stack = stack;
		if (stack)
		{
			meshSpace->SetRenderable(0, stack->GetMesh());
			baseRenderer->SceneChanged();
		}
		else
		{
			meshSpace->SetRenderable(0, 0);
			baseRenderer->SceneChanged();
		}
	}

	// ------------------------------------------------------------------------
	bool ShapeStackRenderer::Initialize()
	{
		bool ok = Superclass::Initialize();
		if (!ok) return false;

		ok = baseRenderer->TryInitialize();
		if (!ok) return false;

		// Set up shader for g-buffer creation
		GLProgram *meshShader = GLProgram::New();
		ok = meshShader != 0;
		if (ok) ok = meshShader->AddVertexShader(
			LoadShader("MeshShaderVS.txt"));
		if (ok) ok = meshShader->AddFragmentShader(
			LoadShader("MeshShaderFS.txt"));
		if (ok) ok = meshShader->Link();
		if (!ok)
		{
			std::cerr << "Error creating mesh shader" << std::endl;
			delete meshShader;
			meshShader = 0;
		}
		meshAttributes = meshShader->GetActiveAttributes();
		NQVTK::SimpleRenderer *renderer = 
			dynamic_cast<NQVTK::SimpleRenderer*>(baseRenderer);
		renderer->SetShader(meshShader);

		// Set up shader for contour detection and compositing
		delete compositeShader;
		compositeShader = GLProgram::New();
		ok = compositeShader != 0;
		if (ok) ok = compositeShader->AddVertexShader(
			LoadShader("CompositeShaderVS.txt"));
		if (ok) ok = compositeShader->AddFragmentShader(
			LoadShader("CompositeShaderFS.txt"));
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
		renderer->SetTarget(meshBuffer);
		renderer->SetViewport(0, 0, 1024, 1024);
		tm->AddTexture("meshBuffer", 
			meshBuffer->GetTexture2D(GL_COLOR_ATTACHMENT0), false);

		return meshShader != 0 && 
			compositeShader != 0 &&
			meshBuffer != 0;
	}
}
