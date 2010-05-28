#include <GLBlaat/GL.h>

#include "MainWindow.h"
#include "MainWindow.moc"

#include "Data/Population.h"
#include "Data/ShapeMesh.h"
#include "Data/ShapeModel.h"

#include "Rendering/SideBySideConfiguration.h"
#include "Rendering/ShapeStackConfiguration.h"
#include "Rendering/OverlayConfiguration.h"

#include <QActionGroup>
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

		// Group the configurations as only one can be active
		QActionGroup *group = new QActionGroup(this);
		group->addAction(ui.actionSideBySide);
		group->addAction(ui.actionOverlaidContours);
		group->addAction(ui.actionShapeStack);
		ui.actionSideBySide->setChecked(true);

		// Group the scatter plot coloring modes
		group = new QActionGroup(this);
		group->addAction(ui.actionPlotColoringOff);
		group->addAction(ui.actionPlotColoringProbability);
		group->addAction(ui.actionPlotColoringReconstructionError);
		ui.actionPlotColoringProbability->setChecked(true);

		// Group the mesh coloring modes
		group = new QActionGroup(this);
		group->addAction(ui.actionMeshColoringDeformation);
		group->addAction(ui.actionMeshColoringReconstructionError);
		ui.actionMeshColoringDeformation->setChecked(true);

		// A user should start by loading a mesh
		ui.actionLoadPopulation->setEnabled(false);

		setWindowTitle(qApp->applicationName());

		connect(ui.plotViewer, SIGNAL(XAxisChanged(itpp::vec)), 
			ui.evolutionViewer, SLOT(SetVector(itpp::vec)));

		connect(ui.plotViewer, SIGNAL(PointSelected(itpp::vec)), 
			ui.meshViewer, SLOT(SetShape(itpp::vec)));

		connect(ui.plotViewer, SIGNAL(NumberOfAxesChanged(int)), 
			ui.meshViewer, SLOT(SetReconstructionDimension(int)));
		connect(ui.plotViewer, SIGNAL(NumberOfAxesChanged(int)), 
			ui.evolutionViewer, SLOT(SetReconstructionDimension(int)));

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
	bool MainWindow::LoadMesh(const QString &filename)
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
			return true;
		}
		return false;
	}

	// ------------------------------------------------------------------------
	bool MainWindow::LoadPopulation(const QString &filename)
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
				return true;
			}
			else
			{
				// The population is useless for the current mesh
				delete newPop;
			}
		}
		return false;
	}

	// ------------------------------------------------------------------------
	void MainWindow::SaveScreenshot(NQVTKWidget *viewer, 
		const QString &filename, bool invertAlpha)
	{
		// TODO: add support for subpixel rendering to the NQVTKViewer
		//QImage screenshot = viewer->grabFrameBuffer(true);
		QImage screenshot = viewer->GrabHighRes(6);
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
	void MainWindow::on_actionSideBySide_triggered()
	{
		ui.evolutionViewer->SetConfiguration(new SideBySideConfiguration());
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionOverlaidContours_triggered()
	{
		ui.evolutionViewer->SetConfiguration(new OverlayConfiguration());
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionShapeStack_triggered()
	{
		ui.evolutionViewer->SetConfiguration(new ShapeStackConfiguration());
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPlotColoringOff_triggered()
	{
		ui.plotViewer->SetColorMode(ScatterPlotRenderer::ColoringOff);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPlotColoringProbability_triggered()
	{
		ui.plotViewer->SetColorMode(ScatterPlotRenderer::ColorByProbability);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPlotColoringReconstructionError_triggered()
	{
		ui.plotViewer->SetColorMode(
			ScatterPlotRenderer::ColorByReconstructionError);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionMeshColoringDeformation_triggered()
	{
		ui.meshViewer->SetColorMode(MeshViewer::ColorByDeformation);
		ui.evolutionViewer->SetColorMode(
			ShapeEvolutionViewer::ColorByDeformation);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionMeshColoringReconstructionError_triggered()
	{
		ui.meshViewer->SetColorMode(MeshViewer::ColorByReconstructionError);
		ui.evolutionViewer->SetColorMode(
			ShapeEvolutionViewer::ColorByReconstructionError);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionMorphShape_triggered()
	{
		ui.evolutionViewer->MorphShape(ui.meshViewer->GetShape());
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionMorphEnd_triggered()
	{
		ui.evolutionViewer->MorphEnd();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionThreshold_toggled(bool checked)
	{
		// TODO: add a slider to configure the threshold value
		ui.evolutionViewer->SetThreshold(checked ? 1.0 : -1.0);
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_evolutionViewer_fpsChanged(int fps)
	{
		setWindowTitle(QString("%1 - %2 fps")
			.arg(qApp->applicationName()).arg(fps));
	}
}
