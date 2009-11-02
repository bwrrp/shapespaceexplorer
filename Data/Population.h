#pragma once

#include <QString>

#include <itpp/itbase.h>

namespace Diverse
{
	class Population
	{
	public:
		static Population *Load(const QString &filename);

		~Population();

		int GetShapeSpaceDimension();
		int GetNumberOfIndividuals();
		itpp::vec GetIndividual(int i);

		// unsigned int GetNumberOfComponents(double threshold);
		// vec GetPrincipalComponent(unsigned int i);

	protected:
		Population(const itpp::mat &population);

		// Population matrix
		itpp::mat population;
	};
}
