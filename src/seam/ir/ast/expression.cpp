#include "expression.hpp"
#include "visitor.hpp"

namespace seam::ir::ast::expression
{
	void bool_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void string_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void unresolved_symbol::visit(visitor* vst)
	{
		vst->visit(this);
	}
	
	void variable::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			value->visit(vst);
		}
	}

	void call::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			function->visit(vst);
			for (const auto& argument : arguments)
			{
				argument->visit(vst);
			}
		}
	}
}