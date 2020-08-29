#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../ir/ast/statement.hpp"

namespace seam::types
{
	struct module
	{
		std::string name;
		std::vector<std::shared_ptr<module>> dependencies;

		std::unique_ptr<ir::ast::statement::restricted_block> body;

		module(std::string name) :
			name(std::move(name))
		{}
	};
}