#include "function_resolver.hpp"
#include "../../ir/ast/type.hpp"
#include "../../utils/exception.hpp"
#include "../../ir/ast/visitor.hpp"
#include "../../ir/ast/expression.hpp"

#include <memory>
#include <sstream>

namespace seam::parser::passes
{
    using namespace ir::ast;

	struct resolver : visitor
	{
		const function_collector::function_map& function_map_;

		bool visit(expression::symbol_wrapper* node) override
		{
			const auto& symbol_name = static_cast<expression::unresolved_symbol*>(node->value.get())->value;
			const auto& it = function_map_.find(symbol_name);
			if (it == function_map_.cend())
			{
				std::stringstream error_message;
				error_message << "cannot resolve symbol '" << symbol_name << '\'';
				throw utils::parser_exception{ node->range.start, error_message.str() };
			}
			node->value = std::make_unique<expression::resolved_symbol>(it->second);
			
			return false;
		}

		resolver(const function_collector::function_map& function_map_) :
			function_map_(function_map_)
		{}
	};

	void function_resolver::run(node* node)
	{
		resolver vst{ function_map_ };
		node->visit(&vst);
	}

	function_resolver::function_resolver(const function_collector::function_map& function_map_) :
		function_map_(function_map_)
	{}
}
