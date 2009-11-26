#pragma once

#include <NQVTK/Rendering/Renderer.h>

class GLProgram;
class GLFramebuffer;

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeStack;
	class MeshRenderer;

	class ShapeEvolutionRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ShapeEvolutionRenderer();
		~ShapeEvolutionRenderer();

		virtual void Draw();

		void SetShapeStack(ShapeStack *stack);

		NQVTK::Camera *GetMeshCamera();

	protected:
		virtual bool Initialize();

		ShapeStack *stack;

		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
		MeshRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;
	};
}
