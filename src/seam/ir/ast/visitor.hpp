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

        VISITOR(node, expression::function_signature);
        VISITOR(node, statement::statement);
        VISITOR(node, expression::expression);
    	
        
		VISITOR(statement::statement, statement::function_definition); 
        VISITOR(statement::statement, statement::class_type_definition);
        VISITOR(statement::statement, statement::alias_type_definition);
        VISITOR(statement::statement, statement::restricted);
        VISITOR(statement::statement, statement::restricted_block);
        VISITOR(statement::statement, statement::normal_block);
        VISITOR(statement::statement, statement::expression_);
        VISITOR(statement::statement, statement::ret);
        VISITOR(statement::statement, statement::while_loop);
        VISITOR(statement::statement, statement::if_stat);
        VISITOR(statement::statement, statement::assignment);
        
        VISITOR(statement::restricted, statement::extern_function_definition);

        VISITOR(expression::expression, expression::variable_ref);
        VISITOR(expression::expression, expression::symbol_wrapper);
        VISITOR(expression::expression, expression::call);
        VISITOR(expression::expression, expression::bool_literal);
        VISITOR(expression::expression, expression::string_literal);
		VISITOR(expression::expression, expression::number_literal);
        VISITOR(expression::expression, expression::binary);
    };
}

#undef VISITOR