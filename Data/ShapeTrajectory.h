#pragma once

#include <itpp/itbase.h>

namespace Diverse
{
	class ShapeTrajectory
	{
	public:
		ShapeTrajectory();
		virtual ~ShapeTrajectory();

		virtual void GetShapes(itpp::vec &offsets, 
			itpp::mat &shapes) const = 0;
	};
}
