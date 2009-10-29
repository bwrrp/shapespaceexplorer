#pragma once

#include <NQVTKGui/Viewports/NQVTKWidget.h>

namespace Diverse
{
	class MainViewer : public NQVTKWidget
	{
		Q_OBJECT;

	public:
		MainViewer(QWidget *parent = 0);
		~MainViewer();
	};
}
