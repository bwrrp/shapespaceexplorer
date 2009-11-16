#include "MeshRenderer.h"

#include "Data/ShapeMesh.h"
#include "Data/Utility.h"

#include <GLBlaat/GLProgram.h>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MeshRenderer::MeshRenderer() : mesh(0)
	{
	}

	// ------------------------------------------------------------------------
	MeshRenderer::~MeshRenderer()
	{
	}

	// ------------------------------------------------------------------------
	void MeshRenderer::Draw()
	{
		if (mesh) mesh->SetShape(shape);

		Superclass::Draw();
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
	bool MeshRenderer::Initialize()
	{
		if (!Superclass::Initialize()) return false;

		// Initialize mesh shader
		GLProgram *meshShader = GLProgram::New();
		bool ok = meshShader != 0;
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
		GLProgram *oldShader = SetShader(meshShader);
		delete oldShader;

		return ok;
	}
}
