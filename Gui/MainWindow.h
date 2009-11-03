#pragma once

#include <QMainWindow>

#include "ui_MainWindow.h"

namespace NQVTK { class Scene; }

namespace Diverse
{
	class Population;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT;

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void RedrawViewers();

	protected:
		Ui::MainWindow ui;

		Population *population;

		NQVTK::Scene *scene;

	private slots:
		void on_actionLoadMesh_triggered();
		void on_actionLoadPopulation_triggered();
		void on_actionPCA_triggered();
		void on_actionQuit_triggered();
		void on_actionReloadShaders_triggered();
		void on_actionStartBenchmark_triggered();
		void on_actionScreenshot_triggered();
		void on_mainViewer_fpsChanged(int fps);
	};
}
