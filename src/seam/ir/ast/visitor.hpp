#pragma once

#include "node.hpp"
#include "statement.hpp"

#define BASE_VISITOR(c) virtual bool visit(c* a) { return true; }
#define VISITOR(b, c) virtual bool visit(c* a) { return visit(static_cast<b*>(a)); }

namespace seam::ir::ast
{
    struct visitor
    {
        virtual ~visitor() = default;
        
        BASE_VISITOR(node);

        VISITOR(node, unresolved_type);
        VISITOR(node, statement::statement);

		VISITOR(statement::statement, statement::function_definition); 
        VISITOR(statement::statement, statement::class_type_definition);
        VISITOR(statement::statement, statement::alias_type_definition);
        VISITOR(statement::statement, statement::restricted);
        VISITOR(statement::statement, statement::restricted_block);
    };
}

#undef VISITOR