#pragma once

#include "pass.hpp"
#include "../../ir/ast/statement.hpp"

#include <memory>
#include <unordered_map>
#include <string>

namespace seam::parser::passes
{
    using function_map = std::unordered_map<std::string, std::shared_ptr<ir::ast::function_signature>>;
	using type_map = std::unordered_map<std::string, std::shared_ptr<ir::ast::resolved_type>>;

    struct type_collector final : pass
	{
		function_map function_map_;
		type_map type_map_;

		void run(ir::ast::node* node) override;
	};
}
