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
    	
        VISITOR(statement::statement, statement::restricted);
        VISITOR(statement::statement, statement::restricted_block);
        VISITOR(statement::statement, statement::block);
        VISITOR(statement::statement, statement::ret);
        VISITOR(statement::statement, statement::variable_declaration);
        VISITOR(statement::statement, statement::variable_assignment);

		VISITOR(statement::restricted, statement::function_definition); 
        VISITOR(statement::restricted, statement::type_definition);

        VISITOR(statement::type_definition, statement::alias_type_definition);
        VISITOR(statement::type_definition, statement::class_type_definition);

        VISITOR(expression::expression, expression::literal);
        VISITOR(expression::expression, expression::unresolved_symbol);
        VISITOR(expression::expression, expression::variable);
        VISITOR(expression::expression, expression::call);

        VISITOR(expression::literal, expression::bool_literal);
        VISITOR(expression::literal, expression::string_literal);
        VISITOR(expression::literal, expression::number_literal);
    };
}

#undef VISITOR