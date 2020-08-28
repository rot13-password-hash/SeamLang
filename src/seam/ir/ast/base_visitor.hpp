#pragma once

#include "node.hpp"
#include "statement.hpp"

#define BASE_VISITOR(c) virtual void base_visit(c* a) {}
#define VISITOR(b, c) virtual void base_visit(c* a) { return base_visit(static_cast<b*>(a)); }

#define WRAP_BASE_VISITOR(c) \
	void base_visit(c* a) override { ret = visit(a); } \
	virtual T visit(c* a) {}

#define WRAP_VISITOR(b, c) \
	void base_visit(c* a) override { ret = visit(a); } \
	virtual T visit(c* a) { return visit(static_cast<b*>(a)); }

namespace seam::ir::ast
{
    struct base_visitor
    {
        virtual ~base_visitor() = default;
        
        BASE_VISITOR(node)

        VISITOR(node, statement)
        
        VISITOR(statement, restricted_statement)
        VISITOR(statement, restricted_block)
    };

    template<typename T>
    class visitor : public base_visitor
    {
		T ret;
	public:
        virtual ~visitor() = default;
        
        WRAP_BASE_VISITOR(node)

        WRAP_VISITOR(node, statement)
        
        WRAP_VISITOR(statement, restricted_statement)
        WRAP_VISITOR(statement, restricted_block)
    };
}

#undef BASE_VISITOR
#undef VISITOR

#undef WRAP_VISITOR