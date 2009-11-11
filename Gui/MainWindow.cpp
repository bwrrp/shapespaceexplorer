#include <GLBlaat/GL.h>

#include "MainWindow.h"
#include "MainWindow.moc"

#include "ShapeStackControls.h"

#include "Data/Population.h"
#include "Data/ShapeMesh.h"
#include "Data/ShapeModel.h"

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QImage>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), 
		model(0)
	{
		ui.setupUi(this);

		// A user should start by loading a mesh
		ui.actionLoadPopulation->setEnabled(false);
		ui.actionPCA->setEnabled(false);

		setWindowTitle(qApp->applicationName());

		stackControls = new ShapeStackControls();
	}

	// ------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete model;
	}

	// ------------------------------------------------------------------------
	void MainWindow::RedrawViewers()
	{
		ui.plotViewer->updateGL();
		ui.stackViewer->updateGL();
	}

	// ------------------------------------------------------------------------
	void MainWindow::LoadMesh(const QString &filename)
	{
		ShapeMesh *mesh = ShapeMesh::Load(filename);
		if (mesh)
		{
			// A new mesh probably means the population is useless
			// Create a new model
			delete model;
			model = new ShapeModel(mesh);
			// TODO: connect signals
			ui.actionLoadPopulation->setEnabled(true);
			ui.actionPCA->setEnabled(false);

			// TODO: refactor NQVTK so ShapeStack can be a Renderable
			// We can't do this yet, as Renderables can't access state 
			// (shaders, textures, attributes) internally. 
			ui.stackViewer->SetShapeModel(model);
			RedrawViewers();
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::LoadPopulation(const QString &filename)
	{
		Population *newPop = Population::Load(filename);
		if (newPop)
		{
			if (model->SetPopulation(newPop))
			{
				ui.plotViewer->SetShapeModel(model);
				ui.actionPCA->setEnabled(true);
				RedrawViewers();
			}
			else
			{
				// The population is useless for the current mesh
				delete newPop;
			}
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionLoadMesh_triggered()
	{
		// Show open dialog
		QString filename = QFileDialog::getOpenFileName(
			this, "Load shape model mesh", QString(), 
			"VTK XML PolyData (*.vtp)");
		// This returns a null string when cancelled
		if (!filename.isNull())
		{
			LoadMesh(filename);
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionLoadPopulation_triggered()
	{
		// Show open dialog
		QString filename = QFileDialog::getOpenFileName(
			this, "Load population vectors", QString(), 
			"Tab-separated text files (*.txt)");
		// This returns a null string when cancelled
		if (!filename.isNull())
		{
			LoadPopulation(filename);
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPCA_triggered()
	{
		// TODO: this should be computed automatically on load
		// TODO: do PCA in a background thread
		Population *population = model->GetPopulation();
		if (population) population->DoPCA();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionQuit_triggered()
	{
		qApp->quit();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionReloadShaders_triggered()
	{
		ui.stackViewer->Initialize();
		RedrawViewers();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionStartBenchmark_triggered()
	{
		ui.stackViewer->StartContinuousUpdate();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionScreenshot_triggered()
	{
		QDateTime now = QDateTime::currentDateTime();
		QImage screenshot = ui.stackViewer->grabFrameBuffer(true);
		// Fix alpha values
		screenshot.invertPixels(QImage::InvertRgba);
		screenshot.invertPixels(QImage::InvertRgb);
		// Save it
		screenshot.save(QString("Diverse-%1.png").arg(
			now.toString("yyMMdd-hhmmss")), "PNG");
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionStackOptions_triggered()
	{
		stackControls->show();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_stackViewer_fpsChanged(int fps)
	{
		setWindowTitle(QString("%1 - %2 fps")
			.arg(qApp->applicationName()).arg(fps));
	}
}
