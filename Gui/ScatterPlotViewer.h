#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <itpp/itbase.h>

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

	signals:
		void PointSelected(itpp::vec shape);
		void XAxisChanged(itpp::vec axis);

	public slots:
		void SyncMeshCamera(NQVTK::Camera *cam);

	protected:
		ShapeModel *model;

	private slots:
		void OnCursorPosChanged(int x, int y);
		void OnProjectionChanged();
	};
}
