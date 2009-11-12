#pragma once

#include <NQVTK/Rendering/NestedRenderer.h>

class GLProgram;
class GLFramebuffer;

namespace Diverse
{
	class ShapeStack;

	class ShapeStackRenderer : public NQVTK::NestedRenderer
	{
	public:
		typedef NQVTK::NestedRenderer Superclass;

		ShapeStackRenderer();
		~ShapeStackRenderer();

		virtual void Clear();

		virtual void Draw();

		virtual void SceneChanged();

		void SetShapeStack(ShapeStack *stack);

	protected:
		virtual bool Initialize();

		ShapeStack *stack;
		NQVTK::Scene *meshSpace;

		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
	};
}
