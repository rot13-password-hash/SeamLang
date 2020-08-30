#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

#include "node.hpp"

namespace seam::ir::ast
{
	struct type : ir::ast::node
	{
		std::string name;
		bool is_optional = false;
		bool is_auto = false;

		explicit type(utils::position_range range, std::string name, bool is_optional, bool is_auto) :
			node(range),
			name(std::move(name)),
			is_optional(is_optional),
			is_auto(is_auto)
		{}
	};

	struct unresolved_type final : type
	{
		void visit(visitor* vst) override;

		unresolved_type(bool is_auto) :
			type({}, "", false, is_auto)
		{}
		
		unresolved_type(utils::position_range range, std::string name, bool is_optional) :
			type(range, name, is_optional, false)
		{}
	};

	struct resolved_type final : type
	{
		explicit resolved_type(utils::position_range range) :
			type(range, {}, false, false) // TODO: check is_auto is fine here
		{}

		bool can_implicitly_cast(resolved_type* other)
		{
			return false;
		}

		bool is_compatible(resolved_type* other)
		{
			return this == other || can_implicitly_cast(other);
		}

		void visit(visitor* vst) override;
	};

	using type_reference = std::variant<unresolved_type>;

	struct parameter
	{
		std::unique_ptr<unresolved_type> param_type;
		std::string name;

		explicit parameter(std::unique_ptr<unresolved_type> param_type, std::string name) :
			param_type(std::move(param_type)), name(std::move(name)) {}
	};

	using parameter_list = std::vector<parameter>;
	using attribute_list = std::unordered_set<std::string>;
}
