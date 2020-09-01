#pragma once

#include "pass.hpp"
#include "../../ir/ast/expression.hpp"

#include <memory>
#include <unordered_map>
#include <string>

namespace seam::parser::passes
{
    struct function_collector final : pass
	{
		using function_map = std::unordered_map<std::string, std::shared_ptr<ir::ast::expression::function_signature>>;
		function_map function_map_;

		void run(ir::ast::node* node) override;
	};
}
