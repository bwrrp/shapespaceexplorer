#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

namespace Diverse
{
	class ShapeStack;

	class ShapeStackViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		ShapeStackViewer(QWidget *parent = 0);
		~ShapeStackViewer();

		void SetShapeStack(ShapeStack *stack);
	};
}
