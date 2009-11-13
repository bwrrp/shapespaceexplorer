#include "ScatterShapesRenderer.h"

#include "Data/Population.h"
#include "Data/ShapeModel.h"
#include "Data/ShapeMesh.h"

#include "Data/Utility.h"

#include <NQVTK/Rendering/Scene.h>
#include <NQVTK/Rendering/SimpleRenderer.h>

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>
#include <GLBlaat/GLTextureManager.h>

namespace Diverse
{
	// ------------------------------------------------------------------------
	ScatterShapesRenderer::ScatterShapesRenderer() : mesh(0), meshBuffer(0)
	{
		meshRenderer = new NQVTK::SimpleRenderer();
		meshSpace = new NQVTK::Scene();
		meshSpace->AddRenderable(0);
		meshRenderer->SetScene(meshSpace);
		meshRenderer->SetDrawBackground(false);
	}

	// ------------------------------------------------------------------------
	ScatterShapesRenderer::~ScatterShapesRenderer()
	{
		delete meshRenderer;
		meshSpace->SetRenderable(0, 0);
		delete meshSpace;
		delete meshBuffer;
	}

	// ------------------------------------------------------------------------
	void ScatterShapesRenderer::SetMesh(ShapeMesh *mesh)
	{
		if (this->mesh == mesh) return;
		this->mesh = mesh;
		meshSpace->SetRenderable(0, mesh);
		// TODO: pre-render shape miniatures for the population
	}

	// ------------------------------------------------------------------------
	bool ScatterShapesRenderer::Initialize()
	{
		if (!Superclass::Initialize()) return false;

		if (!meshRenderer->TryInitialize()) return false;

		bool ok;

		// Initialize mesh shader
		GLProgram *meshShader = GLProgram::New();
		ok = meshShader != 0;
		if (ok) ok = meshShader->AddVertexShader(
			Utility::LoadShader("MeshShaderVS.txt"));
		if (ok) ok = meshShader->AddFragmentShader(
			Utility::LoadShader("MeshShaderFS.txt"));
		if (ok) ok = meshShader->Link();
		if (!ok)
		{
			std::cerr << "Error creating mesh shader" << std::endl;
			delete meshShader;
			meshShader = 0;
		}
		GLProgram *oldShader = meshRenderer->SetShader(meshShader);
		delete oldShader;

		// Initialize mesh FBO
		delete meshBuffer;
		meshBuffer = GLFramebuffer::New(256, 256);
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
		meshRenderer->SetViewport(0, 0, 256, 256);
		tm->AddTexture("meshBuffer", 
			meshBuffer->GetTexture2D(GL_COLOR_ATTACHMENT0), false);

		return meshShader != 0 && meshBuffer != 0;
	}

	// ------------------------------------------------------------------------
	void ScatterShapesRenderer::DrawPoint(const itpp::vec &point)
	{
		// TODO: find out why this doesn't stick...
		meshRenderer->SetScene(meshSpace);

		// Store matrices and state
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		if (fboTarget) fboTarget->Unbind();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		// TODO: Render the mesh
		if (mesh) mesh->SetShape(point);
		meshRenderer->Draw();

		// Restore state and matrices
		glPopAttrib();
		if (fboTarget) fboTarget->Bind();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// TODO: Could use another shader for this
		GLTexture *tex = meshBuffer->GetTexture2D(GL_COLOR_ATTACHMENT0);
		tex->BindToCurrent();
		glEnable(tex->GetTextureTarget());

		// Draw a quad showing the mesh
		NQVTK::Vector3 pos = ProjectPoint(point);
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 1.0);
		glVertex3dv(PosToViewport((pos + 
			NQVTK::Vector3(-1.0, 1.0, 0.0)) * zoom).V);
		glTexCoord2d(1.0, 1.0);
		glVertex3dv(PosToViewport((pos + 
			NQVTK::Vector3(1.0, 1.0, 0.0)) * zoom).V);
		glTexCoord2d(1.0, 0.0);
		glVertex3dv(PosToViewport((pos + 
			NQVTK::Vector3(1.0, -1.0, 0.0)) * zoom).V);
		glTexCoord2d(0.0, 0.0);
		glVertex3dv(PosToViewport((pos + 
			NQVTK::Vector3(-1.0, -1.0, 0.0)) * zoom).V);
		glEnd();

		glDisable(tex->GetTextureTarget());
		tex->UnbindCurrent();
	}
}
