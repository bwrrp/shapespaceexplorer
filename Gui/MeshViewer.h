#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;

	class MeshViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		MeshViewer(QWidget *parent = 0);
		virtual ~MeshViewer();

		void SetShapeModel(ShapeModel *model);

	protected:
		ShapeModel *model;
		NQVTK::Scene *scene;
	};
}
