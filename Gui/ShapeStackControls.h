#pragma once

#include <QWidget>

#include "ui_ShapeStackControls.h"

namespace Diverse
{
	class ShapeMesh;
	class ShapeStack;

	class ShapeStackControls : public QWidget
	{
		Q_OBJECT;

	public:
		ShapeStackControls(QWidget *parent = 0);

		ShapeStack *GetStack(ShapeMesh *mesh);

	signals:
		void StackCreated();
		void StackUpdated();

	protected:
		Ui::ShapeStackControls ui;

		ShapeStack *stack;
	};
}
