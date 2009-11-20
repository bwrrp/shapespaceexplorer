#pragma once

#include <QString>

#include <itpp/itbase.h>

namespace Diverse
{
	class CoordinateFrame;

	class Population
	{
	public:
		static Population *Load(const QString &filename);

		~Population();

		int GetShapeSpaceDimension();
		int GetNumberOfIndividuals();
		itpp::vec GetIndividual(int i);

		void DoPCA();
		CoordinateFrame *GetPrincipalComponentBasis(int dims = 0);
		itpp::mat GetCovariance(int dims = 0);

		Population *TransformTo(CoordinateFrame *basis = 0);

	protected:
		Population(const itpp::mat &population);
		Population(const itpp::mat &population, const itpp::mat &covariance);

		void Center();

		// Population matrix
		itpp::mat population;

		// Principal component basis and covariance
		itpp::mat covariance;
		itpp::mat eigVecs;
		itpp::vec eigVals;
	};
}
