#pragma once

#include <cstddef>

namespace seam::utils
{
	/**
	 * Container for line and column position.
	 */
	struct position
	{
		std::size_t line; // line position
		std::size_t column; // column position
	};

	/**
	 * Struct for specifying a position range.
	 */
	struct position_range
	{
		position start; // start position
		position end; // end position
	};
}