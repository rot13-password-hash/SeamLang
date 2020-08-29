#pragma once

/**
* Mangled function names are in the following format:
* MODULE_NAME@FUNCTION_NAME
*/

#include <vector>
#include <unordered_set>
#include <string>
#include <memory>

#include "../ir/ast/type.hpp"

namespace seam::types
{
    struct function_signature
    {
        std::string name;
        std::unique_ptr<ir::ast::unresolved_type> return_type;
        std::vector<ir::ast::parameter> parameters;
        std::unordered_set<std::string> attributes;

		std::string mangled_name;

        explicit function_signature(std::string module_name, std::string name, std::unique_ptr<ir::ast::unresolved_type> return_type, ir::ast::parameter_list parameters,
                                    ir::ast::attribute_list attributes) :
            name(std::move(name)),
            return_type(std::move(return_type)),
            parameters(std::move(parameters)),
            attributes(std::move(attributes))
        {
			mangled_name = module_name + "@" + this->name;
		}
    };
}
