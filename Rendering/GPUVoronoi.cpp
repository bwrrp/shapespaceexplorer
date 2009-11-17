#include "GPUVoronoi.h"

#include "Data/Utility.h"

#include <GLBlaat/GLFramebuffer.h>
#include <GLBlaat/GLProgram.h>
#include <GLBlaat/GLOcclusionQuery.h>

#include <GLBlaat/GLUtility.h>

#include <cassert>
#include <cmath>
#include <iostream>

namespace Diverse
{
	// ------------------------------------------------------------------------
	GPUVoronoi::GPUVoronoi() : voronoiBuffer(0), coneShader(0)
	{
	}

	// ------------------------------------------------------------------------
	GPUVoronoi::~GPUVoronoi()
	{
		delete voronoiBuffer;
		delete coneShader;
	}

	// ------------------------------------------------------------------------
	bool GPUVoronoi::Initialize()
	{
		delete voronoiBuffer;
		voronoiBuffer = 0;

		delete coneShader;
		coneShader = GLProgram::New();
		bool ok = coneShader != 0;
		if (ok) ok = coneShader->AddVertexShader(
			Utility::LoadShader("ConeShaderVS.txt"));
		if (ok) ok = coneShader->AddFragmentShader(
			Utility::LoadShader("ConeShaderFS.txt"));
		if (ok) ok = coneShader->Link();
		if (!ok)
		{
			std::cerr << "Error creating cone shader" << std::endl;
			delete coneShader;
			coneShader = 0;
		}

		return ok;
	}

	// ------------------------------------------------------------------------
	bool GPUVoronoi::Resize(int w, int h)
	{
		if (voronoiBuffer)
		{
			if (!voronoiBuffer->Resize(w, h))
			{
				delete voronoiBuffer;
				voronoiBuffer = 0;
			}
		}
		if (!voronoiBuffer)
		{
			// TODO: create FBO with depth/stencil
			voronoiBuffer = GLFramebuffer::New(w, h);
			bool ok = voronoiBuffer != 0;
			if (ok)
			{
				voronoiBuffer->CreatePackedDepthStencilBuffer();
				glDrawBuffer(GL_NONE);
				ok = voronoiBuffer->IsOk();
				voronoiBuffer->Unbind();
			}
			if (!ok)
			{
				std::cerr << "Error creating buffer for voronoi rendering" << 
					std::endl;
				delete voronoiBuffer;
				voronoiBuffer = 0;
			}
		}

		width = static_cast<float>(w);
		height = static_cast<float>(h);

		return voronoiBuffer != 0;
	}

	// ------------------------------------------------------------------------
	void GPUVoronoi::UpdateVoronoi(const std::vector<NQVTK::Vector3> &points)
	{
		// Make sure we have enough queries for this set of points
		UpdateQueries(points.size() + 1);

		if (!voronoiBuffer) return;

		// Save state
		PushGLState();

		voronoiBuffer->Bind();
		glClearStencil(0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		// Stencil records cone ID
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glEnable(GL_STENCIL_TEST);
		coneShader->Start();
		coneShader->SetUniform1f("size", std::max(width, height) * sqrt(2.0f));
		coneShader->SetUniform1i("test", 0);
		for (unsigned int i = 0; i < points.size(); ++i)
		{
			// Stencil is cleared to 0, so offset IDs by 1
			glStencilFunc(GL_ALWAYS, i + 1, 0xFFFF);
			DrawConePass(points[i]);
		}
		coneShader->Stop();
		voronoiBuffer->Unbind();

		// Restore state
		PopGLState();
	}

	// ------------------------------------------------------------------------
	std::vector<double> GPUVoronoi::QueryPoint(const NQVTK::Vector3 &point)
	{
		std::vector<double> weights(queries.size() - 1, 0.0);

		if (!voronoiBuffer) return weights;

		// Save state
		PushGLState();

		// Run queries for this point's Voronoi area
		voronoiBuffer->Bind();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glEnable(GL_STENCIL_TEST);
		coneShader->Start();
		coneShader->SetUniform1f("size", std::max(width, height) * sqrt(2.0f));
		coneShader->SetUniform1i("test", 1);
		for (unsigned int i = 0; i < queries.size(); ++i)
		{
			// The first query records the entire area,
			// subsequent queries record overlap with existing cones
			glStencilFunc(i == 0 ? GL_ALWAYS : GL_EQUAL, i, 0xFFFF);
			queries[i]->Start();
			DrawConePass(point);
			queries[i]->Stop();
		}
		coneShader->Stop();
		glDisable(GL_STENCIL_TEST);
		glDepthMask(GL_TRUE);
		voronoiBuffer->Unbind();

		// Restore state
		PopGLState();

		// Collect results
		double refArea = static_cast<double>(queries[0]->GetResultui());
		double check = 0.0;
		for (unsigned int i = 0; i < weights.size(); ++i)
		{
			double result = static_cast<double>(queries[i + 1]->GetResultui());
			weights[i] = result / refArea;
			check += weights[i];
		}
		return weights;
	}

	// ------------------------------------------------------------------------
	void GPUVoronoi::UpdateQueries(unsigned int num)
	{
		if (queries.size() == num) return;

		// Add queries if needed
		while (num > queries.size())
		{
			queries.push_back(GLOcclusionQuery::New());
		}

		// Remove queries if needed
		if (num < queries.size())
		{
			for (unsigned int i = num; i < queries.size(); ++i)
			{
				delete queries[i];
			}
			queries.resize(num);
		}
	}

	// ------------------------------------------------------------------------
	void GPUVoronoi::PushGLState()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
	}

	// ------------------------------------------------------------------------
	void GPUVoronoi::PopGLState()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();
	}

	// ------------------------------------------------------------------------
	void GPUVoronoi::DrawConePass(const NQVTK::Vector3 &point)
	{
		coneShader->SetUniform2f("point", 
			static_cast<float>(point.x), 
			static_cast<float>(point.y));
		// Draw full screen pass
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex3d(-1.0, 1.0, 0.0);
		glTexCoord2d(width, 0.0);
		glVertex3d(1.0, 1.0, 0.0);
		glTexCoord2d(width, height);
		glVertex3d(1.0, -1.0, 0.0);
		glTexCoord2d(0.0, height);
		glVertex3d(-1.0, -1.0, 0.0);
		glEnd();
	}
}
