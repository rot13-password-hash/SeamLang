#include "expression.hpp"

void seam::ir::ast::call_expression::visit(base_visitor* vst)
{
	vst->base_visit(this);
}

void seam::ir::ast::call_expression::visit_children(base_visitor* vst)
{
	function->visit(vst);
	for (const auto& argument : arguments)
	{
		argument->visit(vst);
	}
}

void seam::ir::ast::variable::visit(base_visitor* vst)
{
	vst->base_visit(this);
}

void seam::ir::ast::variable::visit_children(base_visitor* vst)
{
	value->visit(vst);
}

void seam::ir::ast::unresolved_symbol::visit(base_visitor* vst)
{
	vst->base_visit(this);
}
