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
		itpp::vec GetShape() { return shape; }
		void SetReference(itpp::vec shape);
		void SetUseColorMap(bool use);

	protected:
		ShapeMesh *mesh;
		itpp::vec shape;
		itpp::vec reference;

		GLProgram *meshShader;

		GLFramebuffer *meshBuffer;

		bool useColorMap;

		virtual bool Initialize();

		virtual void DrawShadingPass();
	};
}
