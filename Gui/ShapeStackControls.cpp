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

	// ------------------------------------------------------------------------
	ShapeStack *ShapeStackControls::GetStack(ShapeMesh *mesh)
	{
		// TODO: add UI to select the stack type
		stack = new PopulationProjectionStack(mesh);
		return stack;
	}
}
