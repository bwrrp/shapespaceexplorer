#include <GLBlaat/GL.h>

#include "ShapeStackRenderer.h"

#include "Data/ShapeMesh.h"
#include "Data/Population.h"

#include "Rendering/ShapeStack.h"

#include <itpp/itbase.h>

#include <NQVTK/Rendering/Camera.h>
#include <NQVTK/Rendering/Scene.h>

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>

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
		: stack(0), meshShader(0), compositeShader(0), meshBuffer(0)
	{
	}

	// ------------------------------------------------------------------------
	ShapeStackRenderer::~ShapeStackRenderer()
	{
		SetShader(0);
		delete meshShader;
		delete compositeShader;
		delete meshBuffer;
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::Clear()
	{
		Superclass::Clear();
		// TODO: we should integrate this with the NQVTK SimpleRenderer
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		// Blend in the background last
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBegin(GL_QUADS);
		glColor4d(0.2, 0.2, 0.25, 0.0);
		glVertex3d(-1.0, -1.0, 0.0);
		glVertex3d(1.0, -1.0, 0.0);
		glColor4d(0.6, 0.6, 0.65, 0.0);
		glVertex3d(1.0, 1.0, 0.0);
		glVertex3d(-1.0, 1.0, 0.0);
		glEnd();
		glPopAttrib();
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::SetShapeStack(ShapeStack *stack)
	{
		this->stack = stack;
		// TODO: make sure the ShapeMesh is also in the scene!
	}

	// ------------------------------------------------------------------------
	bool ShapeStackRenderer::Initialize()
	{
		bool ok = Superclass::Initialize();
		if (!ok) return false;

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
		meshAttributes = meshShader->GetActiveAttributes();

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

		return meshShader != 0 && 
			compositeShader != 0 &&
			meshBuffer != 0;
	}

	// ------------------------------------------------------------------------
	void ShapeStackRenderer::DrawRenderables()
	{
		// TODO: Add support for adding normal renderables to the scene
		//Superclass::DrawRenderables();

		// Now render the shape stack...
		if (!stack)
		{
			Superclass::DrawRenderables();
			return;
		}

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
			// Render this slice
			int slice = order(i);
			stack->SetupSliceMesh(slice);
			// TODO: setup camera for object (mesh space) rendering
			// TODO: render object to a g-buffer
			meshShader->Start();
			mesh->SetupAttributes(meshAttributes);
			mesh->ApplyParamSets(meshShader, tm);
			mesh->Draw();
			meshShader->Stop();
			// TODO: setup camera for final (composite space) rendering
			double offset = stack->GetSliceOffset(slice);
			// TODO: render slice, detect contours and composite
		}

		// For now, some testing
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
}
