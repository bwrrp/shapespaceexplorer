#include "CoordinateFrame.h"

#include <cassert>

namespace Diverse
{
	// ------------------------------------------------------------------------
	CoordinateFrame::CoordinateFrame(const itpp::mat &basis) : basis(basis)
	{
		// Basis vectors are stored as columns of the matrix
	}

	// ------------------------------------------------------------------------
	CoordinateFrame *CoordinateFrame::IdentityBasis(int dim)
	{
		// Create an identity matrix
		itpp::mat basis = itpp::diag(itpp::ones(dim));
		return new CoordinateFrame(basis);
	}

	// ------------------------------------------------------------------------
	int CoordinateFrame::GetOuterDimension() const
	{
		return basis.rows();
	}

	// ------------------------------------------------------------------------
	int CoordinateFrame::GetInnerDimension() const
	{
		return basis.cols();
	}

	// ------------------------------------------------------------------------
	itpp::vec CoordinateFrame::TransformOut(
		const itpp::vec &pointInFrame) const
	{
		assert(pointInFrame.size() == GetInnerDimension());
		return basis * pointInFrame;
	}

	// ------------------------------------------------------------------------
	itpp::vec CoordinateFrame::TransformIn(
		const itpp::vec &pointInOuter) const
	{
		assert(pointInOuter.size() == GetOuterDimension());
		// NOTE: transpose for inverse only valid for orthogonal basis
		return basis.transpose() * pointInOuter;
	}

	// ------------------------------------------------------------------------
	itpp::mat CoordinateFrame::TransformOut(
		const itpp::mat &pointsInFrame) const
	{
		// Points as rows
		assert(pointsInFrame.cols() == GetInnerDimension());
		return pointsInFrame * basis.transpose();
	}

	// ------------------------------------------------------------------------
	itpp::mat CoordinateFrame::TransformIn(
		const itpp::mat &pointsInOuter) const
	{
		// Points as rows
		assert(pointsInOuter.cols() == GetOuterDimension());
		return pointsInOuter * basis;
	}
}
