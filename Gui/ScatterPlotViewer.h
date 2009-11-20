#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

#include <QKeyEvent>

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
		void SetUsePCAFrame(bool use);

	protected:
		ShapeModel *model;
		bool usePCAFrame;

	private:
		// Qt event handler
		void keyPressEvent(QKeyEvent *event);

	private slots:
		void OnCursorPosChanged(int x, int y);
		void OnProjectionChanged();
	};
}
