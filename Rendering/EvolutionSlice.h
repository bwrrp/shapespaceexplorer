#pragma once

#include <NQVTK/Math/Vector3.h>

#include <itpp/itbase.h>

namespace Diverse
{
	class EvolutionSlice
	{
	public:
		NQVTK::Vector3 origin;
		NQVTK::Vector3 up;
		NQVTK::Vector3 right;

		itpp::vec shape;
		double offset;

		double contourColoring;
		double contourOpacity;
		double shapeOpacity;

		static EvolutionSlice Interpolate(const EvolutionSlice &slice1, 
			const EvolutionSlice &slice2, double t);

		void Draw() const;
	};
}
