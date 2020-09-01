#pragma once

#include "pass.hpp"
#include "function_collector.hpp"

namespace seam::parser::passes
{
	struct function_resolver : pass
	{
		const function_collector::function_map& function_map_;

		void run(ir::ast::node* node) override;

		explicit function_resolver(const function_collector::function_map& function_map_);
	};
}
