#pragma once

#include <itpp/itbase.h>

namespace Diverse
{
	class CoordinateFrame
	{
	public:
		CoordinateFrame(const itpp::mat &basis);

		static CoordinateFrame *IdentityBasis(int dim);
		static CoordinateFrame *PartialIdentityBasis(int fromDim, int toDim);

		int GetOuterDimension() const;
		int GetInnerDimension() const;

		// Single-point transforms
		itpp::vec TransformOut(const itpp::vec &pointInFrame) const;
		itpp::vec TransformIn(const itpp::vec &pointInOuter) const;

		// Multiple-point transforms (points as rows)
		itpp::mat TransformOut(const itpp::mat &pointsInFrame) const;
		itpp::mat TransformIn(const itpp::mat &pointsInOuter) const;

		// For transforming a covariance matrix to the frame's basis
		itpp::mat TransformCovarianceIn(const itpp::mat &outerCov) const;

	protected:
		itpp::mat basis;
	};
}
