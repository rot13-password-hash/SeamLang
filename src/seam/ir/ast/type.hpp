#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <unordered_set>

#include "node.hpp"

namespace seam::ir::ast
{
	struct unresolved_type : ir::ast::node
	{
		std::string name;
		bool is_optional = false;

		unresolved_type()
			: node({}) {}
		
		unresolved_type(const utils::position_range range)
			: node(range) {}

		unresolved_type(const utils::position_range range, std::string name, const bool is_optional)
			: node(range), name(std::move(name)), is_optional(is_optional) {}

		unresolved_type(unresolved_type&& other) noexcept
			: node(other.range),
			name(std::move(other.name)),
			is_optional(other.is_optional) {}

		unresolved_type(unresolved_type& other) = default;

		unresolved_type& operator=(unresolved_type&& other) noexcept
		{
			range = other.range;
			name = std::move(other.name);
			is_optional = other.is_optional;

			return *this;
		}

		void visit(visitor* vst) override;
	};

	struct resolved_type : node
	{
		std::string name;

		resolved_type() :
			node({}) {}
		
		void visit(visitor* vst) override;
	};

	template <typename T>
	struct built_in_type final : resolved_type
	{
		T value;
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
