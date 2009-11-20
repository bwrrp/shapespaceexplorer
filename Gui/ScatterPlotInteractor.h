#pragma once

#include <QObject>

#include <NQVTK/Interactors/Interactor.h>

namespace Diverse
{
	class ScatterPlotRenderer;
	struct PointWidget;

	class ScatterPlotMessenger : public QObject
	{
		Q_OBJECT;

	public:
		void EmitCursorPosChanged(int x, int y);
		void EmitProjectionChanged();

	signals:
		void CursorPosChanged(int x, int y);
		void ProjectionChanged();
	};

	class ScatterPlotInteractor : public NQVTK::Interactor
	{
	public:
		typedef NQVTK::Interactor Superclass;

		ScatterPlotInteractor(ScatterPlotRenderer *renderer);
		virtual ~ScatterPlotInteractor();

		ScatterPlotMessenger *GetMessenger();

		virtual bool MouseMoveEvent(NQVTK::MouseEvent event);
		virtual bool MousePressEvent(NQVTK::MouseEvent event);
		virtual bool MouseReleaseEvent(NQVTK::MouseEvent event);

		void ReleaseWidgets();

	protected:
		ScatterPlotMessenger *messenger;

		ScatterPlotRenderer *renderer;

		PointWidget *activeWidget;

		int lastX;
		int lastY;
		int downX;
		int downY;
		bool drag;

		bool picking;
	};
}
