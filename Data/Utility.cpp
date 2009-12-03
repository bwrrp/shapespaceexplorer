#include "Utility.h"

#include <QApplication>

#include <fstream>
#include <iostream>
#include <sstream>

#include <cmath>

namespace Diverse
{
	namespace Utility
	{
		// --------------------------------------------------------------------
		std::string LoadShader(const std::string &filename)
		{
			// TODO: embed shader sources in the app for final version
			std::string apppath = 
				QApplication::applicationDirPath().toStdString();
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

		// --------------------------------------------------------------------
		NQVTK::Vector3 ToLinearGamma(const NQVTK::Vector3 &color)
		{
			NQVTK::Vector3 v = (color + NQVTK::Vector3(0.055)) / 1.055;
			return NQVTK::Vector3(
				std::pow(v.x, 2.4), 
				std::pow(v.y, 2.4), 
				std::pow(v.z, 2.4));
		}

		// --------------------------------------------------------------------
		NQVTK::Vector3 ToScreenGamma(const NQVTK::Vector3 &color)
		{
			NQVTK::Vector3 v = 1.055 * color;
			return NQVTK::Vector3(
				std::pow(v.x, 1.0 / 2.4), 
				std::pow(v.y, 1.0 / 2.4), 
				std::pow(v.z, 1.0 / 2.4)) - NQVTK::Vector3(0.055);
		}

		// --------------------------------------------------------------------
		NQVTK::Vector3 InterpolateColors(const NQVTK::Vector3 &color1, 
			const NQVTK::Vector3 &color2, double t)
		{
			double invt = 1.0 - t;
			return ToScreenGamma(
				invt * ToLinearGamma(color1) + 
				t * ToLinearGamma(color2));
		}
	}
}
