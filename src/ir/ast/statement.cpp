#include "visitor.hpp"

namespace seam::ir::ast
{
	void restricted_block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}
	
	void restricted_block::visit_children(visitor* vst)
	{
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}
	
	void function_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}
	
	void function_definition::visit_children(visitor* vst)
	{
		body->visit(vst);
	}
	
	void alias_type_definition::visit(visitor* vst)
	{
		vst->visit(this);
	}
	
	void class_type_definition::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}
	
	void class_type_definition::visit_children(visitor* vst)
	{
		body->visit(vst);
	}
	
	void block::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			visit_children(vst);
		}
	}

	void block::visit_children(visitor* vst)
	{
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}
}