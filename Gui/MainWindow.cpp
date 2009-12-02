#include <GLBlaat/GL.h>

#include "MainWindow.h"
#include "MainWindow.moc"

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

		setWindowTitle(qApp->applicationName());

		connect(ui.plotViewer, SIGNAL(XAxisChanged(itpp::vec)), 
			ui.evolutionViewer, SLOT(SetVector(itpp::vec)));

		connect(ui.plotViewer, SIGNAL(PointSelected(itpp::vec)), 
			ui.meshViewer, SLOT(SetShape(itpp::vec)));

		connect(ui.meshViewer, SIGNAL(cameraUpdated(NQVTK::Camera*)), 
			ui.plotViewer, SLOT(SyncMeshCamera(NQVTK::Camera*)));
		connect(ui.meshViewer, SIGNAL(cameraUpdated(NQVTK::Camera*)), 
			ui.evolutionViewer, SLOT(SyncMeshCamera(NQVTK::Camera*)));

		connect(ui.actionUsePCA, SIGNAL(toggled(bool)), 
			ui.plotViewer, SLOT(SetUsePCAFrame(bool)));
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
		ui.evolutionViewer->updateGL();
		ui.meshViewer->updateGL();
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

			// TODO: refactor NQVTK so ShapeStack can be a Renderable
			// We can't do this yet, as Renderables can't access state 
			// (shaders, textures, attributes) internally. 
			ui.evolutionViewer->SetShapeModel(model);
			ui.plotViewer->SetShapeModel(model);
			ui.meshViewer->SetShapeModel(model);
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
				// TODO: implement these as a signal from the model instead
				ui.plotViewer->SetShapeModel(model);
				ui.evolutionViewer->SetShapeModel(model);
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
	void MainWindow::SaveScreenshot(NQVTKWidget *viewer, 
		const QString &filename, bool invertAlpha)
	{
		// TODO: add support for subpixel rendering to the NQVTKViewer
		QImage screenshot = viewer->grabFrameBuffer(true);
		if (invertAlpha)
		{
			// Fix alpha values
			screenshot.invertPixels(QImage::InvertRgba);
			screenshot.invertPixels(QImage::InvertRgb);
		}
		// Save it
		screenshot.save(filename, "PNG");
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
	void MainWindow::on_actionQuit_triggered()
	{
		qApp->quit();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionReloadShaders_triggered()
	{
		ui.plotViewer->Initialize();
		ui.evolutionViewer->Initialize();
		ui.meshViewer->Initialize();
		RedrawViewers();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionStartBenchmark_triggered()
	{
		ui.evolutionViewer->StartContinuousUpdate();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionScreenshot_triggered()
	{
		QString now = QDateTime::currentDateTime().toString("yyMMdd-hhmmss");
		// Save screenshots of all viewers
		SaveScreenshot(ui.plotViewer, 
			QString("Diverse-%1-Plot.png").arg(now));
		SaveScreenshot(ui.meshViewer, 
			QString("Diverse-%1-Mesh.png").arg(now));
		SaveScreenshot(ui.evolutionViewer, 
			QString("Diverse-%1-Evolution.png").arg(now), true);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_evolutionViewer_fpsChanged(int fps)
	{
		setWindowTitle(QString("%1 - %2 fps")
			.arg(qApp->applicationName()).arg(fps));
	}
}
