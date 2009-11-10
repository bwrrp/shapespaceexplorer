#include "Population.h"

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
		// TODO: optimize PCA if # individuals < # shape space dimensions
		// - Build orthonormal basis from individuals 
		//   (normalize, make independent of earlier vectors)
		// - Compute PCA on this basis
		// - Project eigenvectors back to the original shape space

		qDebug("Computing covariance...");

		itpp::mat covariance = itpp::cov(population);

		qDebug("Computing eigenvectors...");

		itpp::vec eigVals;
		itpp::mat eigVecs;

		itpp::eig_sym(covariance, eigVals, eigVecs);

		qDebug("Computing components...");

		assert(eigVals.size() == eigVecs.cols());

		// Sort by the eigVals in decreasing order
		itpp::ivec index = itpp::reverse(itpp::sort_index(eigVals));

		eigComps.set_size(eigVecs.rows(), eigVecs.cols());

		// Multiply the eigVecs by sqrt(eigVals) and remove insignificant ones
		int compcol = 0;
		for (int col = 0; col < eigVecs.cols(); ++col)
		{
			itpp::vec evec = eigVecs.get_col(index(col));
			double eval = eigVals(index(col));
			const double epsilon = 0.000001;
			if (eval > epsilon)
			{
				evec *= sqrt(eval);
				eigComps.set_col(compcol, evec);
				++compcol;
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
		eigComps.set_size(eigVecs.rows(), compcol, true);

		assert(eigComps.rows() == GetShapeSpaceDimension());

		qDebug(QString("PCA complete! Found %1 significant components")
			.arg(compcol).toAscii());
	}

	// ------------------------------------------------------------------------
	int Population::GetNumberOfPrincipalComponents()
	{
		return eigComps.cols();
	}

	// ------------------------------------------------------------------------
	itpp::vec Population::GetPrincipalComponent(int i)
	{
		return eigComps.get_col(i);
	}

	// ------------------------------------------------------------------------
	itpp::vec Population::PointFromComponents(itpp::vec compVec)
	{
		int dims = compVec.size();
		assert(dims <= GetNumberOfPrincipalComponents());
		itpp::vec result(GetShapeSpaceDimension());
		result.zeros();
		for (int i = 0; i < dims; ++i)
		{
			itpp::vec ev = GetPrincipalComponent(i);
			result += compVec(i) * ev / sqrt(itpp::dot(ev, ev));
		}
		return result;
	}

	// ------------------------------------------------------------------------
	itpp::vec Population::ComponentsFromPoint(itpp::vec point, int dims)
	{
		assert(point.size() == GetShapeSpaceDimension());
		if (dims <= 0) dims = GetNumberOfPrincipalComponents();
		itpp::vec result(dims);
		result.zeros();
		// Project point onto eigenvectors
		for (int i = 0; i < dims; ++i)
		{
			itpp::vec ev = GetPrincipalComponent(i);
			result(i) = itpp::dot(point, ev) / sqrt(itpp::dot(ev, ev));
		}
		return result;
	}

	// ------------------------------------------------------------------------
	Population *Population::ReduceDimensionality(int dims)
	{
		assert(dims <= GetNumberOfPrincipalComponents());
		if (dims <= 0) dims = GetNumberOfPrincipalComponents();
		int numIndividuals = GetNumberOfIndividuals();
		itpp::mat result(numIndividuals, dims);
		for (int i = 0; i < numIndividuals; ++i)
		{
			result.set_row(i, ComponentsFromPoint(GetIndividual(i), dims));
		}
		return new Population(result);
	}
}
