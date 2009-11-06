#pragma once

#include <NQVTK/Rendering/NestedRenderer.h>

#include <GLBlaat/GLShaderInfo.h>

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

		GLProgram *meshShader;
		GLProgram *compositeShader;

		// Cached attributes required by the mesh shader
		std::vector<GLAttributeInfo> meshAttributes;

		GLFramebuffer *meshBuffer;
	};
}
