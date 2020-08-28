#pragma once

#include <string>
#include <variant>

namespace seam::ir::ast
{
	struct type final
	{
		std::string name;
		bool is_optional = false;

		type() = default;

		type(std::string name, const bool is_optional)
			:	name(std::move(name)), is_optional(is_optional) {}

		type(type&& other) noexcept
		:	name(std::move(other.name)),
			is_optional(other.is_optional) {}

		type(type& other) = default;

		type& operator=(type&& other) noexcept
		{
			name = std::move(other.name);
			is_optional = other.is_optional;

			return *this;
		}
	};

	using type_reference = std::variant<type>;
}
