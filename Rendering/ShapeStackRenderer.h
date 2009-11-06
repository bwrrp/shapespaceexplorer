#pragma once

#include <NQVTK/Rendering/SimpleRenderer.h>

class GLProgram;
class GLFramebuffer;

namespace Diverse
{
	class ShapeStack;

	class ShapeStackRenderer : public NQVTK::SimpleRenderer
	{
	public:
		typedef NQVTK::SimpleRenderer Superclass;

		ShapeStackRenderer();
		~ShapeStackRenderer();

		virtual void Clear();

		void SetShapeStack(ShapeStack *stack);

	protected:
		virtual bool Initialize();

		virtual void DrawRenderables();

		ShapeStack *stack;

		GLProgram *meshShader;
		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
	};
}
