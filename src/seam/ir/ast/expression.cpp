#include "expression.hpp"

namespace seam::ir::ast::expression
{
	void unresolved_symbol::visit(visitor* vst)
	{
		vst->visit(this);
	}
	
	void variable::visit(visitor* vst)
	{
		vst->visit(this);
		value->visit(vst);
	}

	void call::visit(visitor* vst)
	{
		vst->visit(this);
		function->visit(vst);
		for (const auto& argument : arguments)
		{
			argument->visit(vst);
		}
	}
}