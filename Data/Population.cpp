#include "Population.h"

#include <itpp/itsignal.h>

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
}