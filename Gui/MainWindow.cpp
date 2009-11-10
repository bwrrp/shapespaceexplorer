#include <GLBlaat/GL.h>

#include "MainWindow.h"
#include "MainWindow.moc"

#include "ShapeStackControls.h"

#include "Data/Population.h"
#include "Data/ShapeMesh.h"

#include "Rendering/ShapeStack.h"
#include "Rendering/PopulationProjectionStack.h"
#include "Rendering/ShapeSpaceLineStack.h"

#include "Rendering/ShapeStackRenderer.h"

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
		population(0), stack(0)
	{
		ui.setupUi(this);

		// A user should start by loading a mesh
		ui.actionLoadPopulation->setEnabled(false);
		ui.actionPCA->setEnabled(false);

		scene = new NQVTK::Scene();
		ui.stackViewer->GetRenderer()->SetScene(scene);

		setWindowTitle(qApp->applicationName());

		stackControls = new ShapeStackControls();
	}

	// ------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
		delete stack;
		// The mesh is deleted as part of the scene
		delete scene;
		delete population;
	}

	// ------------------------------------------------------------------------
	void MainWindow::RedrawViewers()
	{
		ui.plotViewer->updateGL();
		ui.stackViewer->updateGL();
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
				// TODO: refactor NQVTK so ShapeStack can be a Renderable
				// We can't do this yet, as Renderables can't access state 
				// (shaders, textures, attributes) internally. 
				// TODO: add UI for selecting the stack type
				delete stack;
				//stack = stackControls->GetStack(mesh);
				stack = new PopulationProjectionStack(mesh);
				//stack = new ShapeSpaceLineStack(mesh);
				ui.stackViewer->SetShapeStack(stack);
				RedrawViewers();
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
				if (stack->SetPopulation(newPop))
				{
					delete population;
					population = newPop;
					ui.plotViewer->SetPopulation(population);
					ui.actionPCA->setEnabled(true);
					// TODO: Add UI for (eigen)vector selection
					if (population->GetNumberOfIndividuals() > 0)
					{
						dynamic_cast<PopulationProjectionStack*>(
							stack)->SetVector(population->GetIndividual(0));
					}
					RedrawViewers();
				}
				else
				{
					// The population is useless for the current mesh
					delete newPop;
				}
			}
		}
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionPCA_triggered()
	{
		// TODO: this should be computed automatically on load
		// TODO: do PCA in a background thread
		if (population) population->DoPCA();

		// TODO: Add UI for (eigen)vector selection
		if (population->GetNumberOfPrincipalComponents() > 0)
		{
			dynamic_cast<PopulationProjectionStack*>(stack)->SetVector(
				population->GetPrincipalComponent(0));
			// TODO: this currently leaks memory!
			ui.plotViewer->SetPopulation(population->ReduceDimensionality());
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
