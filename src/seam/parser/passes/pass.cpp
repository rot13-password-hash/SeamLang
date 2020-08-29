#include "pass.hpp"

#include "symbol_resolver.hpp"

namespace seam::parser::passes
{
    void pass::run_passes(ir::ast::node* root)
    {
        // resolve symbols (types and functions)
        symbol_collector symbol_collector_;
        symbol_collector_.run(root);
        symbol_resolver symbol_resolver_{ symbol_collector_ };
        symbol_resolver_.run(root);
    }
}