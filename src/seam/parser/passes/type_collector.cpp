#include "type_collector.hpp"
#include "../../utils/exception.hpp"
#include "../../ir/ast/visitor.hpp"

#include <sstream>

namespace seam::parser::passes
{
    using namespace ir::ast;
    
    static const std::unordered_map<std::string, resolved_type::built_in_type> built_in_types
    {
        { "void", resolved_type::built_in_type::void_ },
        { "bool", resolved_type::built_in_type::bool_ },
        { "string", resolved_type::built_in_type::string },
        { "i8", resolved_type::built_in_type::i8 },
        { "i16", resolved_type::built_in_type::i16 },
        { "i32", resolved_type::built_in_type::i32 },
        { "i64", resolved_type::built_in_type::i64 },
        { "u8", resolved_type::built_in_type::u8 },
        { "u16", resolved_type::built_in_type::u16 },
        { "u32", resolved_type::built_in_type::u32 },
        { "u64", resolved_type::built_in_type::u64 },
        { "f32", resolved_type::built_in_type::f32 },
        { "f64", resolved_type::built_in_type::f64 }
    };

    struct collector : visitor
	{
		function_map& function_map_;
		type_map& type_map_;

		void register_type(utils::position_range range, const std::string& name, std::shared_ptr<resolved_type> t)
        {
            if (type_map_.find(name) != type_map_.cend())
            {
                std::stringstream error_message;
                error_message << "cannot redefine type '" << name << '\'';
                throw utils::parser_exception{ {0, 0}, error_message.str() };
            }

            type_map_.emplace(name, t);
        }

		std::shared_ptr<resolved_type> collect_type(utils::position_range range, const std::string& name)
        {
            auto resolved = std::make_shared<resolved_type>();
        
            const auto& built_in_it = built_in_types.find(name);
            if (built_in_it != built_in_types.cend())
            {
                resolved->value = built_in_it->second;
            }
            else
            {
                const auto& t = type_map_.find(name);
                if (t != type_map_.cend())
                {
                    resolved->value = t->second->value;
                }
                else
                {
                    std::stringstream error_message;
                    error_message << "cannot use undefined type '" << name << '\'';
                    throw utils::parser_exception{ range.start, error_message.str() };
                }
            }

            type_map_.emplace(name, resolved);
            return resolved;
        }

		bool visit(statement::alias_type_definition* node) override
        {
            const auto resolved = collect_type(node->range, node->target_type->value->name);
            register_type(node->range, node->name, resolved);
            return false;
        }

        bool visit(type_wrapper* node) override
		{
            const auto& name = node->value->name;
            const auto& it = type_map_.find(name);
            if (it == type_map_.cend())
            {
                collect_type(node->range, name);
            }

            return false;
		}

		bool visit(statement::class_type_definition* node) override
        {
            const auto class_type = std::make_shared<resolved_type>();
            class_type->value = class_descriptor{};

            if (!node->fields.empty())
            {
                throw utils::parser_exception{ node->range.start, "class fields are not supported" };
            }

            register_type(node->range, node->name, class_type);
            return false;
        }

		explicit collector(function_map& function_map_, type_map& type_map_) :
            function_map_(function_map_),
            type_map_(type_map_)
        {}
	};

    void type_collector::run(node* node)
    {
	    collector vst{function_map_, type_map_};
	    node->visit(&vst);
    }
}
