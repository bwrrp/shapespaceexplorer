#include <QApplication>
#include <QStringList>

#include "Gui/MainWindow.h"

#ifdef WIN32
#include <vtkWin32ProcessOutputWindow.h>
#endif

int main(int argc, char *argv[]) 
{
#ifdef WIN32
	// Use a more useful output window for VTK
	vtkWin32ProcessOutputWindow *ow = vtkWin32ProcessOutputWindow::New();
	vtkWin32ProcessOutputWindow::SetInstance(ow);
	ow->Delete();
#endif

	// Set up Qt application
	QApplication app(argc, argv);
	app.setApplicationName("Diverse");

	Diverse::MainWindow *window = new Diverse::MainWindow();

	window->show();

	// If a model was specified on the command line, load it
	QStringList args = app.arguments();
	if (args.size() > 1)
	{
		bool ok = window->LoadMesh(args[1]);

		if (ok && args.size() > 2)
		{
			window->LoadPopulation(args[2]);
		}
	}

	// Go!
	app.exec();

	// Clean up and exit
	delete window;
	return 0;
}
