#pragma once

#include <stdexcept>
#include <string>

#include "position.h"

namespace seam::utils
{
	/**
	 * Seam exception class
	 */
	struct exception : std::runtime_error
	{
		utils::position position;

		explicit exception(const utils::position position, const std::string& msg) :
			std::runtime_error(msg.c_str()), position(position)
		{}
	};

	/**
	 * Seam lexical exception class
	 */
	struct lexical_exception final : exception
	{
		explicit lexical_exception(const utils::position position, const std::string& msg) :
			exception(position, msg) {}
	};

	/**
	 * Seam parser exception class
	 */
	struct parser_exception final : exception
	{
		explicit parser_exception(const utils::position position, const std::string& msg) :
			exception(position, msg) {}
	};
}