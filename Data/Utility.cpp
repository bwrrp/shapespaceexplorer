#include "Utility.h"

#include <QApplication>

#include <fstream>
#include <iostream>
#include <sstream>

namespace Diverse
{
	namespace Utility
	{
		std::string LoadShader(const std::string &filename)
		{
			// TODO: embed shader sources in the app for final version
			std::string apppath = QApplication::applicationDirPath().toStdString();
			// Check up to 4 levels up in the directory hierarchy
			std::string searchpath = "/Shaders/" + filename;
			std::ifstream infile((apppath + searchpath).c_str());
			for (int i = 0; i < 4 && !infile; ++i)
			{
				searchpath =  "/.." + searchpath;
				infile.close();
				infile.clear();
				infile.open((apppath + searchpath).c_str());
			}
			std::ostringstream contents;
			if (infile.good())
			{
				contents << infile.rdbuf();
			}
			else
			{
				std::cerr << "Error loading shader!" << std::endl;
			}
			return contents.str();
		}
	}
}
