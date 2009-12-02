#pragma once

#include <QMainWindow>

#include "ui_MainWindow.h"

namespace NQVTK { class Scene; }

namespace Diverse
{
	class ShapeModel;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT;

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void RedrawViewers();

		bool LoadMesh(const QString &filename);
		bool LoadPopulation(const QString &filename);

	protected:
		Ui::MainWindow ui;

		ShapeModel *model;

		void SaveScreenshot(NQVTKWidget *viewer, const QString &filename, 
			bool invertAlpha = false);

	private slots:
		void on_actionLoadMesh_triggered();
		void on_actionLoadPopulation_triggered();
		void on_actionQuit_triggered();
		void on_actionReloadShaders_triggered();
		void on_actionStartBenchmark_triggered();
		void on_actionScreenshot_triggered();
		void on_actionSideBySide_triggered();
		void on_actionOverlaidContours_triggered();
		void on_actionShapeStack_triggered();
		void on_evolutionViewer_fpsChanged(int fps);
	};
}
