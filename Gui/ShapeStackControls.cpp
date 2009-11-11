#include "ShapeStackControls.h"
#include "ShapeStackControls.moc"

#include "Rendering/ShapeStack.h"
#include "Rendering/PopulationProjectionStack.h"
#include "Rendering/ShapeSpaceLineStack.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	ShapeStackControls::ShapeStackControls(QWidget *parent) 
		: QWidget(parent, Qt::Tool), stack(0)
	{
		ui.setupUi(this);
	}
}
