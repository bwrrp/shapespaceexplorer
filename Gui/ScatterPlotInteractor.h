#pragma once

#include <NQVTK/Interactors/Interactor.h>

namespace Diverse
{
	class ScatterPlotRenderer;

	// TODO: add messenger class later to emit signals

	class ScatterPlotInteractor : public NQVTK::Interactor
	{
	public:
		typedef NQVTK::Interactor Superclass;

		ScatterPlotInteractor(ScatterPlotRenderer *renderer);
		virtual ~ScatterPlotInteractor();

		virtual bool MouseMoveEvent(NQVTK::MouseEvent event);
		virtual bool MousePressEvent(NQVTK::MouseEvent event);
		virtual bool MouseReleaseEvent(NQVTK::MouseEvent event);

	protected:
		ScatterPlotRenderer *renderer;
	};
}
