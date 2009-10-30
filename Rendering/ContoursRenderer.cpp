#include <GLBlaat/GL.h>

#include "ContoursRenderer.h"

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>

#include <QApplication>

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
	ContoursRenderer::ContoursRenderer() 
		: meshShader(0), compositeShader(0), meshBuffer(0)
	{
	}

	// ------------------------------------------------------------------------
	ContoursRenderer::~ContoursRenderer()
	{
		delete meshShader;
		delete compositeShader;
		delete meshBuffer;
	}

	// ------------------------------------------------------------------------
	void ContoursRenderer::SetViewport(int x, int y, int w, int h)
	{
		Superclass::SetViewport(x, y, w, h);

		// Resize or create mesh rendering FBO
		if (!meshBuffer)
		{
			meshBuffer = GLFramebuffer::New(w, h);
			meshBuffer->CreateDepthBuffer();
			meshBuffer->CreateColorTexture();
			GLenum bufs[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, bufs);
			if (!meshBuffer->IsOk())
			{
				std::cerr << "Error creating buffer for mesh rendering" << 
					std::endl;
			}
			meshBuffer->Unbind();
		}
		else
		{
			if (!meshBuffer->Resize(w, h))
			{
				std::cerr << 
					"WARNING! Could not resize buffer for mesh rendering!" << 
					std::endl;
			}
		}
	}

	// ------------------------------------------------------------------------
	void ContoursRenderer::Draw()
	{
		// TODO: determine how many contours to draw
		// TODO: determine drawing order
		// TODO: for each object...
		// TODO:     setup camera for object (mesh space) rendering
		// TODO:     render object to a g-buffer
		// TODO:     setup camera for final (composite space) rendering
		// TODO:     detect contours and composite into final scene
	}

	// ------------------------------------------------------------------------
	bool ContoursRenderer::Initialize()
	{
		bool ok;
		// Set up shader for g-buffer creation
		delete meshShader;
		meshShader = GLProgram::New();
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

		// Framebuffer for mesh rendering is created on resize
		delete meshBuffer;
		meshBuffer = 0;

		return meshShader != 0 && compositeShader != 0;
	}
}
