#pragma once

#include <QMainWindow>

#include "ui_MainWindow.h"

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeStackControls;

	class ShapeModel;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT;

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void RedrawViewers();

		void LoadMesh(const QString &filename);
		void LoadPopulation(const QString &filename);

	protected:
		Ui::MainWindow ui;

		ShapeStackControls *stackControls;

		ShapeModel *model;

	private slots:
		void on_actionLoadMesh_triggered();
		void on_actionLoadPopulation_triggered();
		void on_actionPCA_triggered();
		void on_actionQuit_triggered();
		void on_actionReloadShaders_triggered();
		void on_actionStartBenchmark_triggered();
		void on_actionScreenshot_triggered();
		void on_actionStackOptions_triggered();
		void on_stackViewer_fpsChanged(int fps);
	};
}
