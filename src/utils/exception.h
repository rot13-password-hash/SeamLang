#pragma once

#include <stdexcept>
#include <string>

#include "position.h"

namespace seam::utils
{
	struct exception : std::runtime_error
	{
		utils::position position;

		explicit exception(const utils::position position, const std::string& msg) :
			std::runtime_error(msg.c_str()), position(position)
		{}
	};

	struct lexical_exception final : exception
	{
		explicit lexical_exception(const utils::position position, const std::string& msg) :
			exception(position, msg) {}
	};
}