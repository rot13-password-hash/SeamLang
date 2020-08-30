#pragma once

#include "type_collector.hpp"

namespace seam::parser::passes
{
	struct type_resolver : pass
	{
		const type_map& type_map_;

		void run(ir::ast::node* node) override;

		explicit type_resolver(const type_map& type_map_);
	};
}
