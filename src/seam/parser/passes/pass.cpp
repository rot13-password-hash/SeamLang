#include "pass.hpp"

#include "type_collector.hpp"
#include "type_resolver.hpp"
#include "number_resolver.hpp"

namespace seam::parser::passes
{
    void pass::run_passes(ir::ast::node* root)
    {
        // resolve symbols (types and functions)
        type_collector type_collector_;
        type_collector_.run(root);

        type_resolver type_resolver_{ type_collector_.type_map_ };
        type_resolver_.run(root);

        number_resolver number_resolver_;
        number_resolver_.run(root);
    }
}