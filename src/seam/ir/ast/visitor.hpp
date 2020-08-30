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

        VISITOR(node, function_signature);
        VISITOR(node, type_wrapper);
        VISITOR(node, statement::statement);
        VISITOR(node, expression::expression);
    	
		VISITOR(statement::statement, statement::function_definition); 
        VISITOR(statement::statement, statement::class_type_definition);
        VISITOR(statement::statement, statement::alias_type_definition);
        VISITOR(statement::statement, statement::restricted);
        VISITOR(statement::statement, statement::restricted_block);
        VISITOR(statement::statement, statement::block);
        VISITOR(statement::statement, statement::ret);

        VISITOR(expression::expression, expression::call);
        VISITOR(expression::expression, expression::bool_literal);
    };
}

#undef VISITOR