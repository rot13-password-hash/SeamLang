#include "type.hpp"
#include "visitor.hpp"

void seam::ir::ast::type_wrapper::visit(visitor* vst)
{
	vst->visit(this);
}

void seam::ir::ast::function_signature::visit(visitor* vst)
{
	if (vst->visit(this))
	{
		return_type->visit(vst);
		for (const auto& param : parameters)
		{
			param.param_type->visit(vst);
		}
	}
}
