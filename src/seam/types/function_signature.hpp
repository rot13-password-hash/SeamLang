#pragma once

#include "type.hpp"

#include <vector>
#include <unordered_set>
#include <string>

#include "../ir/ast/type.hpp"

namespace seam::types
{
    struct function_signature
    {
        std::string name;
        std::unique_ptr<ir::ast::unresolved_type> return_type;
        std::vector<ir::ast::parameter> parameters;
        std::unordered_set<std::string> attributes;

        function_signature(function_signature&& other) noexcept :
            name(std::move(other.name)),
            return_type(std::move(other.return_type)),
            parameters(std::move(parameters)),
            attributes(std::move(attributes))
        {}

        explicit function_signature(std::string name, std::unique_ptr<ir::ast::unresolved_type> return_type, ir::ast::parameter_list parameters,
                                    ir::ast::attribute_list attributes) :
            name(std::move(name)),
            return_type(std::move(return_type)),
            parameters(std::move(parameters)),
            attributes(std::move(attributes))
        {}
    };
}
