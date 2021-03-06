#pragma once

#include <NQVTK/Rendering/Renderer.h>

#include "EvolutionSlice.h"

class GLProgram;
class GLFramebuffer;

namespace NQVTK { class Scene; }

namespace Diverse
{
	class CoordinateFrame;
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
		void SetReferenceBasis(CoordinateFrame *basis);
		void SetThreshold(float threshold);

		void UpdateSlices();

		NQVTK::Camera *GetMeshCamera();

	protected:
		virtual bool Initialize();

		GLProgram *compositeShader;

		GLFramebuffer *meshBuffer;
		MeshRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;

		ShapeTrajectory *trajectory;
		EvolutionConfiguration *configuration;
		std::vector<EvolutionSlice> slices;

		CoordinateFrame *referenceBasis;

		float threshold;
	};
}
