#include "expression.h"

void seam::ir::ast::call_expression::visit(visitor* vst)
{
	if (vst->visit(this))
	{
		visit_children(vst);
	}
}

void seam::ir::ast::call_expression::visit_children(visitor* vst)
{
	function->visit(vst);
	for (const auto& argument : arguments)
	{
		argument->visit(vst);
	}
}

void seam::ir::ast::variable::visit(visitor* vst)
{
	if (vst->visit(this))
	{
		visit_children(vst);
	}
}

void seam::ir::ast::variable::visit_children(visitor* vst)
{
	value->visit(vst);
}

void seam::ir::ast::unresolved_variable::visit(visitor* vst)
{
	vst->visit(this);
}
