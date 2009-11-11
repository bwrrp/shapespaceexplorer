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
		void EmitProjectionChanged();

	signals:
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

	protected:
		ScatterPlotMessenger *messenger;

		ScatterPlotRenderer *renderer;

		PointWidget *activeWidget;

		int lastX;
		int lastY;
	};
}
