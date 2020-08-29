#pragma once

#include "../../ir/ast/node.hpp"

namespace seam::parser::passes
{
    struct pass
    {
        virtual void run(ir::ast::node* node) = 0;
        virtual ~pass() = default;

        static void run_passes(ir::ast::node* root);
    };
}
