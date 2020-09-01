#pragma once

#include "pass.hpp"
#include "../../ir/ast/node.hpp"

namespace seam::parser::passes
{
	struct types : pass
	{
		void run(ir::ast::node* node) override;
	};
}
