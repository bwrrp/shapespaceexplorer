#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

namespace Diverse
{
	class ShapeModel;

	class ScatterPlotViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		ScatterPlotViewer(QWidget *parent = 0);
		virtual ~ScatterPlotViewer();

		void SetShapeModel(ShapeModel *model);

	protected:
		ShapeModel *model;
	};
}
