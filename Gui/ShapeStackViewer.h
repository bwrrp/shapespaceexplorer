#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <itpp/itbase.h>

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;
	class ShapeStack;

	class ShapeStackViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		ShapeStackViewer(QWidget *parent = 0);
		~ShapeStackViewer();

		void SetShapeModel(ShapeModel *model);

	public slots:
		void SetVector(itpp::vec vector);

	protected:
		ShapeModel *model;
		ShapeStack *stack;
		NQVTK::Scene *scene;
	};
}
