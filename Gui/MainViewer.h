#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

namespace Diverse
{
	class ShapeStack;

	class MainViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		MainViewer(QWidget *parent = 0);
		~MainViewer();

		void SetShapeStack(ShapeStack *stack);
	};
}
