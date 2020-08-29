#pragma once

#include "pass.hpp"
#include "../../ir/ast/visitor.hpp"
#include "../../utils/exception.hpp"

#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>

namespace seam::parser::passes
{
    using namespace ir::ast;
    using function_map = std::unordered_map<std::string, std::shared_ptr<statement::function_definition>>;
	using type_map = std::unordered_map<std::string, std::shared_ptr<resolved_type>>;

	struct collector : visitor
	{
		function_map& function_map_;
		type_map& type_map_;

		void register_type(utils::position_range range, const std::string& name)
		{
			if (type_map_.find(name) != type_map_.cend())
			{
				std::stringstream error_message;
				error_message << "cannot redefine type '" << name << "'";
				throw utils::parser_exception{ range.start, error_message.str() };
			}

			type_map_.insert({ name, std::make_shared<resolved_type>(range) });
		}

		bool visit(statement::function_definition* node) override
		{
			std::cout << "mangled name: " << node->signature->mangled_name << std::endl;
			return true;
		}

		bool visit(statement::alias_type_definition* node) override
		{
			register_type(node->range, node->name);
		}

		bool visit(statement::class_type_definition* node) override
		{
			register_type(node->range, node->name);
		}

		explicit collector(function_map& function_map_, type_map& type_map_) :
			function_map_(function_map_),
			type_map_(type_map_)
		{}
	};

    struct symbol_collector final : pass
	{
		function_map function_map_;
		type_map type_map_;

        void run(node* node) override
        {
            collector vst{ function_map_, type_map_ };
            node->visit(&vst);
        };
	};
}
