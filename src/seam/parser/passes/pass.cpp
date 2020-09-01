#include "pass.hpp"

#include "function_collector.hpp"
#include "function_resolver.hpp"
#include "types.hpp"

namespace seam::parser::passes
{
    void pass::run_passes(ir::ast::node* root)
    {
        // resolve symbols (types and functions)
        function_collector function_collector_;
		function_collector_.run(root);

        function_resolver function_resolver_{ function_collector_.function_map_ };
		function_resolver_.run(root);

		types types_;
		types_.run(root);
    }
}