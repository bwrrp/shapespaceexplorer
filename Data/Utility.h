#pragma once

#include <string>

#include <NQVTK/Math/Vector3.h>

namespace Diverse
{
	namespace Utility
	{
		std::string LoadShader(const std::string &filename);

		// Color manipulation functions
		NQVTK::Vector3 ToLinearGamma(const NQVTK::Vector3 &color);
		NQVTK::Vector3 ToScreenGamma(const NQVTK::Vector3 &color);
		NQVTK::Vector3 InterpolateColors(const NQVTK::Vector3 &color1, 
			const NQVTK::Vector3 &color2, double t);
	}
}
