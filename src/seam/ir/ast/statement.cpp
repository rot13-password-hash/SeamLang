#include "visitor.hpp"

namespace seam::ir::ast::statement
{
	void restricted_block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			for (const auto& statement : body)
			{
				statement->visit(vst);
			}	
		}
	}
	
	void function_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			signature->visit(vst);
			body->visit(vst);
		}
	}
	
	void normal_block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			for (const auto& statement : body)
			{
				statement->visit(vst);
			}
		}
	}

	void expression_::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			value->visit(vst);
		}
	}

	void ret::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			if (value)
			{
				value->visit(vst);
			}
		}
	}

	void assignment::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			from->visit(vst);
			to->visit(vst);
		}
	}

	void if_stat::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			condition->visit(vst);
			main_body->visit(vst);
			if (else_body)
			{
				else_body->visit(vst);
			}
		}
	}
	
	void loop::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			body->visit(vst);
		}
	}

	void numerical_for_loop::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			initial->visit(vst);
			final->visit(vst);
			step->visit(vst);
		}
	}

	void while_loop::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			condition->visit(vst);
		}
	}

	void extern_function_definition::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void alias_type_definition::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void class_type_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			body->visit(vst);
		}
	}
}