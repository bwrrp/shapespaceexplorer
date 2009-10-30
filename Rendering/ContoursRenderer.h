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

		virtual void Draw();

	protected:
		virtual bool Initialize();

		GLProgram *meshShader;
		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
	};
}
