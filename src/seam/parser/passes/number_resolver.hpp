#pragma once
#include "pass.hpp"

namespace seam::parser::passes
{
	struct number_resolver : pass
	{
		void run(ir::ast::node* node) override;

		explicit number_resolver();
	};
}
