#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "pass.hpp"
#include "../../ir/ast/type.hpp"
#include "../../ir/ast/visitor.hpp"

#include <iostream>

namespace seam::parser::passes
{
    using namespace ir;
    using type_map = std::unordered_map<std::string, std::shared_ptr<ast::resolved_type>>;

    struct symbol_collector final : pass
	{
		type_map type_map_;

        struct visitor : ast::visitor
        {
            type_map& type_map_;

            void visit(ast::unresolved_type* node) override
            {
                const auto& name = node->name;
                if (type_map_.find(name) == type_map_.cend())
                {
                    type_map_.insert({ name, std::make_shared<ast::resolved_type>() });
                }
            }

            void visit(ast::statement::alias_type_definition* node) override
            {
                const auto& name = node->name;
                if (type_map_.find(name) == type_map_.cend())
                {
                    type_map_.insert({ name, std::make_shared<ast::resolved_type>() });
                }
            }

            explicit visitor(type_map& type_map_) :
                type_map_(type_map_)
            {}
        };

        void run(ast::node* node) override
        {
            visitor vst{ type_map_ };
            node->visit(&vst);
        };
	};

	struct symbol_resolver final : pass
	{
		const symbol_collector& collector;

        void run(ast::node* node) override
        {
            
        };

        explicit symbol_resolver(symbol_collector& collector) :
            collector(collector)
        {}
	};
}
