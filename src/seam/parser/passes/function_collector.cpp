#include "function_collector.hpp"
#include "../../utils/exception.hpp"
#include "../../ir/ast/statement.hpp"
#include "../../ir/ast/visitor.hpp"

#include <sstream>

namespace seam::parser::passes
{
    using namespace ir::ast;

    struct collector : visitor
	{
		function_collector::function_map& function_map_;

        bool visit(statement::extern_function_definition* node) override
        {
            function_map_.emplace(node->signature->name, node->signature);
            return false;
        }

        bool visit(statement::function_definition* node) override
		{
            function_map_.emplace(node->signature->name, node->signature);
            return true;
		}
    	
		explicit collector(function_collector::function_map& function_map_) :
            function_map_(function_map_)
        {}
	};

    void function_collector::run(node* node)
    {
	    collector vst{ function_map_ };
	    node->visit(&vst);
    }
}
