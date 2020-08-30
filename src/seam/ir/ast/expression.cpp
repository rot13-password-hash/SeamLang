#include "expression.hpp"
#include "visitor.hpp"

namespace seam::ir::ast::expression
{
	void variable::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			type_->visit(vst);
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

	void bool_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void string_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}
	
	void number_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

    void number_wrapper::visit(visitor* vst)
    {
		vst->visit(this);
    }
}