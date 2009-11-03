#pragma once

#include <NQVTK/Rendering/SimpleRenderer.h>

class GLProgram;
class GLFramebuffer;

namespace Diverse
{
	class Population;

	class ContoursRenderer : public NQVTK::SimpleRenderer
	{
	public:
		typedef NQVTK::SimpleRenderer Superclass;

		ContoursRenderer();
		~ContoursRenderer();

		virtual void Clear();

		virtual void Draw();

		// TODO: testing only!
		Population *pop;

	protected:
		virtual bool Initialize();

		GLProgram *meshShader;
		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
	};
}
