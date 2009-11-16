#pragma once

#include <NQVTK/Rendering/SimpleRenderer.h>

#include <itpp/itbase.h>

namespace Diverse
{
	class ShapeMesh;

	class MeshRenderer : public NQVTK::SimpleRenderer
	{
	public:
		typedef NQVTK::SimpleRenderer Superclass;

		MeshRenderer();
		virtual ~MeshRenderer();

		virtual void Draw();

		void SetMesh(ShapeMesh *mesh);
		void SetShape(itpp::vec shape);

	protected:
		ShapeMesh *mesh;
		itpp::vec shape;

		virtual bool Initialize();
	};
}
