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

		explicit type(utils::position_range range, std::string name, bool is_optional) :
			node(range),
			name(std::move(name)),
			is_optional(is_optional)
		{}
	};

	struct unresolved_type final : type
	{
		void visit(visitor* vst) override;

		unresolved_type(utils::position_range range, std::string name, bool is_optional) :
			type(range, name, is_optional)
		{}
	};

	struct resolved_type final : type
	{
		explicit resolved_type(utils::position_range range) :
			type(range, {}, false)
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
