#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

#include "node.hpp"

namespace seam::ir::ast
{
	struct type
	{
		std::string name;
		bool is_optional = false;
		bool is_auto = false;

		explicit type(std::string name, bool is_optional, bool is_auto) :
			name(std::move(name)),
			is_optional(is_optional),
			is_auto(is_auto)
		{}
	};

	struct unresolved_type final : type
	{
		unresolved_type(bool is_auto) :
			type("", false, is_auto)
		{}
		
		unresolved_type(std::string name, bool is_optional) :
			type(name, is_optional, false)
		{}
	};

	struct class_descriptor
	{
		
	};

	struct resolved_type final : type
	{
		enum class built_in_type
		{
			void_,
			bool_,
			string,
			i8,
			i16,
			i32,
			i64,
			u8,
			u16,
			u32,
			u64,
			f32,
			f64
		};

		std::variant<built_in_type, class_descriptor> value;

		bool can_implicitly_cast(resolved_type* other)
		{
			return false;
		}

		bool is_compatible(resolved_type* other)
		{
			return this == other || can_implicitly_cast(other);
		}

		explicit resolved_type() :
			type({}, false, false) {}
	};

	struct type_wrapper final : node
	{
		std::shared_ptr<type> value;

		void visit(visitor* vst) override;

		type_wrapper(utils::position_range range, std::shared_ptr<type> value) :
			node(range), value(std::move(value))
		{}
	};

	struct parameter
	{
		std::unique_ptr<type_wrapper> param_type;
		std::string name;

		explicit parameter(std::unique_ptr<type_wrapper> param_type, std::string name) :
			param_type(std::move(param_type)), name(std::move(name)) {}
	};

	using parameter_list = std::vector<parameter>;
	using attribute_list = std::unordered_set<std::string>;

	struct function_signature : node
	{
		std::string name;
		std::unique_ptr<type_wrapper> return_type;
		std::vector<parameter> parameters;
		std::unordered_set<std::string> attributes;

		std::string mangled_name;

		explicit function_signature(std::string module_name, std::string name, std::unique_ptr<ir::ast::type_wrapper> return_type, ir::ast::parameter_list parameters,
			ir::ast::attribute_list attributes) :
			node({0,0}),
			name(std::move(name)),
			return_type(std::move(return_type)),
			parameters(std::move(parameters)),
			attributes(std::move(attributes))
		{
			mangled_name = module_name + "@" + this->name;
		}

		void visit(visitor* vst) override;
	};
}
