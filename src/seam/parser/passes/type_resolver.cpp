#include "type_resolver.hpp"
#include "../../ir/ast/type.hpp"

namespace seam::parser::passes
{
    using namespace ir::ast;

	struct resolver : visitor
	{
		const type_map& type_map_;

		bool visit(type_wrapper* node) override
		{
			const auto& it = type_map_.find(static_cast<unresolved_type*>(node->value.get())->name);
			if (it != type_map_.cend())
			{
				node->value = it->second;
			}
			else
			{
				throw utils::parser_exception{ node->range.start, "internal compiler error: cannot resolve type" };
			}

			return false;
		}

		resolver(const type_map& type_map_) :
			type_map_(type_map_)
		{}
	};

	void type_resolver::run(node* node)
	{
		resolver vst{ type_map_ };
		node->visit(&vst);
	}

	type_resolver::type_resolver(const type_map& type_map_) :
		type_map_(type_map_)
	{}
}
