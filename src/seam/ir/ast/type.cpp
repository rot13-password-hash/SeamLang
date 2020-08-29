#include "type.hpp"
#include "visitor.hpp"

void seam::ir::ast::unresolved_type::visit(visitor* vst)
{
	vst->visit(this);
}

void seam::ir::ast::resolved_type::visit(visitor* vst)
{
	vst->visit(this);
}
