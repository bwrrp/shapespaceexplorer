#pragma once

#include <NQVTK/Rendering/SimpleRenderer.h>

#include <itpp/itbase.h>

class GLFramebuffer;
class GLProgram;

namespace Diverse
{
	class ShapeMesh;

	class MeshRenderer : public NQVTK::SimpleRenderer
	{
	public:
		typedef NQVTK::SimpleRenderer Superclass;

		MeshRenderer();
		virtual ~MeshRenderer();

		virtual void SetViewport(int x, int y, int w, int h);

		virtual void Draw();

		void SetMesh(ShapeMesh *mesh);
		void SetShape(itpp::vec shape);

	protected:
		ShapeMesh *mesh;
		itpp::vec shape;

		GLProgram *meshShader;

		GLFramebuffer *meshBuffer;

		virtual bool Initialize();

		virtual void DrawShadingPass();
	};
}
