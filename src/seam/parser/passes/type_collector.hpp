#pragma once

#include "pass.hpp"
#include "../../ir/ast/visitor.hpp"
#include "../../utils/exception.hpp"

#include <memory>
#include <unordered_map>
#include <string>
#include <sstream>

namespace seam::parser::passes
{
    using function_map = std::unordered_map<std::string, std::shared_ptr<ir::ast::statement::function_definition>>;
	using type_map = std::unordered_map<std::string, std::shared_ptr<ir::ast::resolved_type>>;

    struct type_collector final : pass
	{
		function_map function_map_;
		type_map type_map_;

		void run(ir::ast::node* node) override;
	};
}
