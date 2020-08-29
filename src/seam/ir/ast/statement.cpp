#include "visitor.hpp"

namespace seam::ir::ast::statement
{
	void restricted_block::visit(visitor* vst)
	{
		vst->visit(this);
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}
	
	void function_definition::visit(visitor* vst)
	{
		vst->visit(this);
		body->visit(vst);
	}
	
	void extern_function_definition::visit(visitor* vst)
	{
		vst->visit(this);
	}
	
	void alias_type_definition::visit(visitor* vst)
	{
		vst->visit(this);
		target_type->visit(vst);
	}
	
	void class_type_definition::visit(visitor* vst)
	{
		vst->visit(this);
		body->visit(vst);
	}
	
	void block::visit(visitor* vst)
	{
		vst->visit(this);
		
		for (const auto& statement : body)
		{
			statement->visit(vst);
		}
	}

	void ret::visit(visitor* vst)
	{
		vst->visit(this);
		value->visit(vst);
	}

}