#pragma once

#include <NQVTK/Rendering/Renderer.h>

class GLProgram;
class GLFramebuffer;

namespace Diverse
{
	class ContoursRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ContoursRenderer();
		~ContoursRenderer();

		virtual void SetViewport(int x, int y, int w, int h);

		virtual void Draw();

	protected:
		virtual bool Initialize();

		GLProgram *meshShader;
		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
	};
}
