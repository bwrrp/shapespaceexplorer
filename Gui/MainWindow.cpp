#include <GLBlaat/GL.h>

#include "MainWindow.h"
#include "MainWindow.moc"

#include "Data/Population.h"
#include "Data/ShapeMesh.h"

#include "Rendering/ContoursRenderer.h"

#include <NQVTK/Rendering/Renderer.h>
#include <NQVTK/Rendering/Scene.h>

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QImage>

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), 
		population(0)
	{
		ui.setupUi(this);

		// A user should start by loading a mesh
		ui.actionLoadPopulation->setEnabled(false);
		ui.actionPCA->setEnabled(false);

		setWindowTitle(qApp->applicationName());

		scene = new NQVTK::Scene();
		ui.mainViewer->GetRenderer()->SetScene(scene);
	}

	// ------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete scene;
		delete population;
	}

	// ------------------------------------------------------------------------
	void MainWindow::RedrawViewers()
	{
		ui.mainViewer->updateGL();
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
			ShapeMesh *mesh = ShapeMesh::Load(filename);
			if (mesh)
			{
				// A new mesh probably means the population is useless
				delete population;
				population = 0;
				ui.actionLoadPopulation->setEnabled(true);
				ui.actionPCA->setEnabled(false);
				// Use the new mesh
				scene->DeleteAllRenderables();
				scene->AddRenderable(mesh);
				ui.mainViewer->GetRenderer()->SceneChanged();
			}
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
			Population *newPop = Population::Load(filename);
			if (newPop)
			{
				delete population;
				population = newPop;

				// TODO: remove after testing
				ContoursRenderer *ren = dynamic_cast<ContoursRenderer*>(
					ui.mainViewer->GetRenderer());
				if (ren) ren->pop = newPop;
			}
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPCA_triggered()
	{
		// TODO: this should be computed automatically on load
		// TODO: do PCA in a background thread
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
		ui.mainViewer->Initialize();
		RedrawViewers();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionStartBenchmark_triggered()
	{
		ui.mainViewer->StartContinuousUpdate();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionScreenshot_triggered()
	{
		QDateTime now = QDateTime::currentDateTime();
		QImage screenshot = ui.mainViewer->grabFrameBuffer(true);
		// Fix alpha values
		screenshot.invertPixels(QImage::InvertRgba);
		screenshot.invertPixels(QImage::InvertRgb);
		// Save it
		screenshot.save(QString("VFE-%1.png").arg(
			now.toString("yyMMdd-hhmmss")), "PNG");
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_mainViewer_fpsChanged(int fps)
	{
		setWindowTitle(QString("%1 - %2 fps")
			.arg(qApp->applicationName()).arg(fps));
	}
}
