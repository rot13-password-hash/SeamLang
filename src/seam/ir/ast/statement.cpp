#include "base_visitor.hpp"

namespace seam::ir::ast
{
	void restricted_block::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}
	
	void restricted_block::visit_children(base_visitor* vst)
	{
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}
	
	void function_definition::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}
	
	void function_definition::visit_children(base_visitor* vst)
	{
		body->visit(vst);
	}

	void extern_function_definition::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}
	
	void alias_type_definition::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}
	
	void class_type_definition::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}
	
	void class_type_definition::visit_children(base_visitor* vst)
	{
		body->visit(vst);
	}
	
	void block::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}

	void block::visit_children(base_visitor* vst)
	{
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}
	
	void return_statement::visit(base_visitor* vst)
	{
		vst->base_visit(this);
	}

	void return_statement::visit_children(base_visitor* vst)
	{
		value->visit(vst);
	}

}