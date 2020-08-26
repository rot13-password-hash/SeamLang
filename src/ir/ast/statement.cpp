#include "visitor.h"

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
}