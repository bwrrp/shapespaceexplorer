#pragma once

#include <NQVTK/Rendering/Renderer.h>

class GLProgram;
class GLFramebuffer;

namespace NQVTK
{
	class Scene;
	class SimpleRenderer;
}

namespace Diverse
{
	class ShapeStack;

	class ShapeStackRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ShapeStackRenderer();
		~ShapeStackRenderer();

		virtual void Draw();

		void SetShapeStack(ShapeStack *stack);

	protected:
		virtual bool Initialize();

		ShapeStack *stack;

		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
		NQVTK::SimpleRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;
	};
}
