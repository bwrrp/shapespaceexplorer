#include "Population.h"

#include "CoordinateFrame.h"

#include <itpp/itsignal.h>
#include <itpp/itstat.h>

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <cassert>
#include <cmath>

namespace Diverse
{
	// ------------------------------------------------------------------------
	Population::Population(const itpp::mat &population) 
		: population(population)
	{
		// Compute mean
		int dim = GetShapeSpaceDimension();
		itpp::vec mean(dim);
		for (int i = 0; i < dim; ++i)
		{
			mean(i) = itpp::mean(population.get_col(i));
		}
		// Subtract mean from all individuals
		int num = GetNumberOfIndividuals();
		for (int i = 0; i < num; ++i)
		{
			this->population.set_row(i, population.get_row(i) - mean);
		}
	}

	// ------------------------------------------------------------------------
	Population::~Population()
	{
	}

	// ------------------------------------------------------------------------
	Population *Population::Load(const QString &filename)
	{
		// Load the population matrix from a tab-separated text file
		itpp::mat population;

		QFile inFile(filename);
		if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qDebug("Could not open population file!");
			return 0;
		}

		// Determine size of data
		int numCols = 0;
		int numRows = 0;
		QString line;
		QTextStream stream(&inFile);
		while (!stream.atEnd())
		{
			line = stream.readLine();
			++numRows;
		}
		QStringList cols = line.split('\t', QString::SkipEmptyParts);
		numCols = cols.size();

		// Resize the matrix and reset the stream
		population.set_size(numRows, numCols);
		stream.seek(0);

		// Load the population file into the matrix
		bool ok = true;
		for (int row = 0; row < numRows; ++row)
		{
			line = stream.readLine();
			QStringList cols = line.split('\t', QString::SkipEmptyParts);
			if (cols.size() != numCols) 
			{
				qDebug("Shape space dimension is not constant across the "
					"population!");
				return 0;
			}
			assert(cols.size() == numCols);
			for (int col = 0; col < numCols; ++col)
			{
				population(row, col) = cols[col].toDouble(&ok);
				if (!ok)
				{
					qDebug(QString(
						"Population file contains invalid value "
						"at (%1, %2): '%3'")
						.arg(row)
						.arg(col)
						.arg(cols[col]).toAscii());
					return 0;
				}
			}
		}

		// TODO: center population on its mean

		return new Population(population);
	}

	// ------------------------------------------------------------------------
	int Population::GetShapeSpaceDimension()
	{
		return population.cols();
	}

	// ------------------------------------------------------------------------
	int Population::GetNumberOfIndividuals()
	{
		return population.rows();
	}

	// ------------------------------------------------------------------------
	itpp::vec Population::GetIndividual(int i)
	{
		return population.get_row(i);
	}

	// ------------------------------------------------------------------------
	void Population::DoPCA()
	{
		itpp::mat pop;
		itpp::mat basis;
		// Optimize PCA if # individuals < # shape space dimensions
		int numIndividuals = GetNumberOfIndividuals();
		int numDimensions = GetShapeSpaceDimension();
		if (numIndividuals < numDimensions)
		{
			qDebug("Reducing dimensionality prior to PCA");

			// Build orthonormal basis from individuals
			basis.set_size(numDimensions, numIndividuals);
			for (int i = 0; i < numIndividuals; ++i)
			{
				itpp::vec indiv = GetIndividual(i);
				// Make orthogonal to earlier vectors
				for (int j = 0; j < i; ++j)
				{
					itpp::vec basisVec = basis.get_col(j);
					indiv -= itpp::dot(indiv, basisVec) * basisVec;
				}
				// Normalize vector
				indiv /= sqrt(itpp::dot(indiv, indiv));
				// Add to basis
				basis.set_col(i, indiv);
			}

			// Transform population to the new basis
			pop = population * basis;
		}
		else
		{
			// Perform PCA on the original population vectors
			pop = population;
		}

		qDebug("Computing covariance...");

		itpp::mat covariance = itpp::cov(pop);

		qDebug("Computing eigenvectors...");

		itpp::vec unsortedEigVals;
		itpp::mat unsortedEigVecs;

		itpp::eig_sym(covariance, unsortedEigVals, unsortedEigVecs);

		if (numIndividuals < numDimensions)
		{
			// Project eigenvectors back to the original shape space
			unsortedEigVecs = basis * unsortedEigVecs;
		}

		qDebug("Sorting components...");

		assert(eigVals.size() == eigVecs.cols());

		// Sort by the eigVals in decreasing order
		itpp::ivec index = itpp::reverse(itpp::sort_index(unsortedEigVals));

		// Allocate space
		eigVals.set_size(unsortedEigVals.size());
		eigVecs.set_size(unsortedEigVecs.rows(), unsortedEigVecs.cols());

		// Sort and remove insignificant pairs
		int comp = 0;
		for (int col = 0; col < eigVecs.cols(); ++col)
		{
			itpp::vec evec = unsortedEigVecs.get_col(index(col));
			double eval = unsortedEigVals(index(col));
			const double epsilon = 0.000001;
			if (eval > epsilon)
			{
				eigVecs.set_col(comp, evec);
				eigVals(comp) = eval;
				++comp;
			}
			else
			{
				if (eval < -epsilon)
				{
					qDebug(QString("Warning! Negative eigenvalue: %1")
						.arg(eval).toAscii());
				}
				// Treat this eigenvalue as if it's 0 and discard the result
			}
		}
		// Discard zero-valued components
		eigVecs.set_size(eigVecs.rows(), comp, true);
		eigVals.set_size(comp, true);

		assert(eigVecs.rows() == GetShapeSpaceDimension());

		qDebug(QString("PCA complete! Found %1 significant components")
			.arg(comp).toAscii());
	}

	// ------------------------------------------------------------------------
	CoordinateFrame *Population::GetPrincipalComponentBasis(int dims)
	{
		assert(dims <= eigVecs.cols());
		if (dims <= 0) dims = eigVecs.cols();
		return new CoordinateFrame(
			eigVecs.get(0, GetShapeSpaceDimension() - 1, 0, dims - 1));
	}

	// ------------------------------------------------------------------------
	double Population::GetComponentVariance(int i)
	{
		return eigVals(i);
	}

	// ------------------------------------------------------------------------
	Population *Population::TransformTo(CoordinateFrame *basis)
	{
		CoordinateFrame *frame = basis;
		if (!basis) frame = GetPrincipalComponentBasis();
		itpp::mat result = frame->TransformIn(population);
		if (!basis) delete frame;
		return new Population(result);
	}
}
