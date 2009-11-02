#include "Population.h"

#include "ShapeModel.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <cassert>

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
}
