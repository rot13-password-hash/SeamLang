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
	 * TODO: Comment this properly.
	 */
	struct position_range
	{
		position start;
		position end;
	};
}