#include "MeshRenderer.h"

#include "Data/ShapeMesh.h"
#include "Data/Utility.h"

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>
#include <GLBlaat/GLTextureManager.h>
#include <GLBlaat/GLUtility.h>

#include <NQVTK/Rendering/Camera.h>

#include <cassert>
#include <sstream>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MeshRenderer::MeshRenderer() : mesh(0), meshShader(0), meshBuffer(0)
	{
		useColorMap = true;
	}

	// ------------------------------------------------------------------------
	MeshRenderer::~MeshRenderer()
	{
		SetShader(0);
		delete meshShader;
		delete meshBuffer;
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::SetViewport(int x, int y, int w, int h)
	{
		Superclass::SetViewport(x, y, w, h);

		if (meshBuffer)
		{
			if (!meshBuffer->Resize(w, h))
			{
				delete meshBuffer;
				meshBuffer = 0;
			}
		}
		if (!meshBuffer)
		{
			// Create G-buffer FBO
			meshBuffer = GLFramebuffer::New(w, h);
			bool ok = meshBuffer != 0;
			if (ok)
			{
				meshBuffer->CreateDepthBuffer();
				int nBufs = 2;
				GLenum bufs[] = {
					GL_COLOR_ATTACHMENT0, 
					GL_COLOR_ATTACHMENT1
				};
				for (int i = 0; i < nBufs; ++i)
				{
					meshBuffer->CreateColorTextureRectangle(
						bufs[i], GL_RGBA16F_ARB, GL_RGBA, GL_FLOAT);
					GLTexture *buf = meshBuffer->GetTexture2D(bufs[i]);
					GLUtility::SetDefaultColorTextureParameters(buf);
				}
				glDrawBuffers(nBufs, bufs);
				ok = meshBuffer->IsOk();
				meshBuffer->Unbind();
			}
			if (!ok)
			{
				std::cerr << "Error creating mesh G-buffer" << std::endl;
				delete meshBuffer;
				meshBuffer = 0;
			}
		}

		// Re-register the textures as they might have moved
		if (meshBuffer)
		{
			tm->AddTexture("gbuffer0", meshBuffer->GetTexture2D(
				GL_COLOR_ATTACHMENT0), false);
			tm->AddTexture("gbuffer1", meshBuffer->GetTexture2D(
				GL_COLOR_ATTACHMENT1), false);
		}
		else
		{
			tm->RemoveTexture("gbuffer0");
			tm->RemoveTexture("gbuffer1");
		}
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::Draw()
	{
		if (mesh) mesh->SetShape(shape);

		// TODO: add a standard deferred shading renderer to NQVTK

		// Fallback in case something went wrong in initialization
		if (!meshBuffer)
		{
			Clear();
			return;
		}

		// G-buffer creation pass
		GLFramebuffer *oldTarget = SetTarget(meshBuffer);
		bool oldDrawBackground = drawBackground;
		SetDrawBackground(false);
		glDisable(GL_BLEND);
		Superclass::Draw();

		// Prepare for the shading pass
		SetTarget(oldTarget);
		SetDrawBackground(oldDrawBackground);

		DrawShadingPass();
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::SetMesh(ShapeMesh *mesh)
	{
		this->mesh = mesh;
		if (mesh)
		{
			shape.set_size(mesh->GetShapeSpaceDimension());
			shape.zeros();
		}
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::SetShape(itpp::vec shape)
	{
		assert(mesh != 0);
		assert(shape.size() == mesh->GetShapeSpaceDimension());
		this->shape = shape;
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::SetUseColorMap(bool use)
	{
		this->useColorMap = use;
	}

	// ------------------------------------------------------------------------
	bool MeshRenderer::Initialize()
	{
		if (!Superclass::Initialize()) return false;

		bool ok;

		// Initialize shader for G-buffer creation
		GLProgram *meshScribe = GLProgram::New();
		ok = meshScribe != 0;
		if (ok) ok = meshScribe->AddVertexShader(
			Utility::LoadShader("MeshScribeVS.txt"));
		if (ok) ok = meshScribe->AddFragmentShader(
			Utility::LoadShader("MeshScribeFS.txt"));
		if (ok) ok = meshScribe->Link();
		if (!ok)
		{
			std::cerr << "Error creating mesh scribe" << std::endl;
			delete meshScribe;
			meshScribe = 0;
		}
		GLProgram *oldShader = SetShader(meshScribe);
		delete oldShader;

		// Initialize shader for deferred shading
		delete meshShader;
		meshShader = GLProgram::New();
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

		// The G-buffer FBO is created when the viewport is resized
		tm->RemoveTexture("gbuffer0");
		tm->RemoveTexture("gbuffer1");
		delete meshBuffer;
		meshBuffer = 0;

		return ok;
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::DrawShadingPass()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (fboTarget) fboTarget->Bind();

		Clear();

		DrawCamera();

		meshShader->Start();
		meshShader->SetUniform1f("viewportX", viewportX);
		meshShader->SetUniform1f("viewportY", viewportY);
		meshShader->SetUniform3f("cameraPos", 
			static_cast<float>(camera->position.x), 
			static_cast<float>(camera->position.y), 
			static_cast<float>(camera->position.z));
		meshShader->SetUniform1i("useColorMap", useColorMap ? 1 : 0);
		tm->SetupProgram(meshShader);
		tm->Bind();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		// Draw full-screen quad
		glBegin(GL_QUADS);
		glVertex3d(-1.0, 1.0, 0.0);
		glVertex3d(1.0, 1.0, 0.0);
		glVertex3d(1.0, -1.0, 0.0);
		glVertex3d(-1.0, -1.0, 0.0);
		glEnd();

		tm->Unbind();
		meshShader->Stop();

		if (fboTarget) fboTarget->Unbind();
		glPopAttrib();
	}
}
