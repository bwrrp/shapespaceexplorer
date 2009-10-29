#include "MainWindow.h"
#include "MainWindow.moc"

#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QImage>

namespace Diverse
{
	// ------------------------------------------------------------------------
	MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
	{
		ui.setupUi(this);
	}

	// ------------------------------------------------------------------------
	MainWindow::~MainWindow()
	{
	}

	// ------------------------------------------------------------------------
	void MainWindow::RedrawViewers()
	{
		ui.mainViewer->updateGL();
	}

	// ------------------------------------------------------------------------
	void MainWindow::on_actionLoad_triggered()
	{
		// Show open dialog
		QString filename = QFileDialog::getOpenFileName(
			this, "Load shape model", QString(), 
			"VTK XML PolyData (*.vtp)");
		// This returns a null string when cancelled
		if (!filename.isNull())
		{
			// TODO: load
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
		setWindowTitle(QString("Diverse - %1 fps").arg(fps));
	}
}
