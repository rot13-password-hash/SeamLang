#pragma once

#include "node.hpp"
#include "statement.hpp"

#define visitOR(c) virtual void visit(c* a) {}
#define VISITOR(b, c) virtual void visit(c* a) { return visit(static_cast<b*>(a)); }

#define WRAP_visitOR(c) \
	void visit(c* a) override { ret = visit(a); } \
	virtual T visit(c* a) {}

#define WRAP_VISITOR(b, c) \
	void visit(c* a) override { ret = visit(a); } \
	virtual T visit(c* a) { return visit(static_cast<b*>(a)); }

namespace seam::ir::ast
{
    struct visitor
    {
        virtual ~visitor() = default;
        
        visitOR(node)

        VISITOR(node, statement::statement)
        
        VISITOR(statement::statement, statement::restricted)
        VISITOR(statement::statement, statement::restricted_block)
    };
}

#undef visitOR
#undef VISITOR

#undef WRAP_VISITOR