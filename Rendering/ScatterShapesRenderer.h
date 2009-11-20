#pragma once

#include "ScatterPlotRenderer.h"

class GLProgram;
class GLFramebuffer;

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeMesh;
	class MeshRenderer;

	class ScatterShapesRenderer : public ScatterPlotRenderer
	{
	public:
		typedef ScatterPlotRenderer Superclass;

		ScatterShapesRenderer();
		virtual ~ScatterShapesRenderer();

		void SetMesh(ShapeMesh *mesh);

		NQVTK::Camera *GetMeshCamera();

		virtual void ZoomToFit();

	protected:
		virtual bool Initialize();

		virtual void DrawPoint(int individual);

		ShapeMesh *mesh;

		GLFramebuffer *meshBuffer;
		MeshRenderer *meshRenderer;
		NQVTK::Scene *meshSpace;

		double shapeSize;
	};
}
