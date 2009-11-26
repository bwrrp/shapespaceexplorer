#include "EvolutionConfiguration.h"

namespace Diverse
{
	// ------------------------------------------------------------------------
	EvolutionConfiguration::EvolutionConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	EvolutionConfiguration::~EvolutionConfiguration()
	{
	}

	// ------------------------------------------------------------------------
	double* EvolutionConfiguration::GetBounds()
	{
		// Subclasses should compute bounds in SetupSlices
		return bounds;
	}
}
