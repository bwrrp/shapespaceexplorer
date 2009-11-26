#pragma once

#include <NQVTK/Rendering/Renderer.h>

#include "EvolutionSlice.h"

class GLProgram;
class GLFramebuffer;

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeMesh;
	class MeshRenderer;

	class ShapeTrajectory;
	class EvolutionConfiguration;

	class ShapeEvolutionRenderer : public NQVTK::Renderer
	{
	public:
		typedef NQVTK::Renderer Superclass;

		ShapeEvolutionRenderer();
		~ShapeEvolutionRenderer();

		virtual void Draw();

		void SetMesh(ShapeMesh *mesh);
		void SetTrajectory(ShapeTrajectory *trajectory);
		void SetConfiguration(EvolutionConfiguration *configuration);

		void UpdateSlices();

		NQVTK::Camera *GetMeshCamera();

	protected:
		virtual bool Initialize();

		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
		MeshRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;
		ShapeMesh *mesh;

		ShapeTrajectory *trajectory;
		EvolutionConfiguration *configuration;
		std::vector<EvolutionSlice> slices;
	};
}
