#pragma once

#include <vector>
#include <memory>
#include <string>
#include <variant>

#include "node.hpp"

namespace seam::ir::ast
{
	struct visitor;
}

namespace seam::ir::ast::expression
{
	struct expression : node
	{
		explicit expression(const utils::position_range range)
			: node(range) {}

		virtual ~expression() = default;
	};

	using expression_list = std::vector<std::unique_ptr<expression>>;

	struct literal : expression
	{
		void visit(visitor* vst) override = 0;

		explicit literal(utils::position_range range) :
			expression(range)
		{}
	};

	struct bool_literal final : literal
	{
		bool value;

		explicit bool_literal(utils::position_range range, bool value) :
			literal(range), value(value)
		{}

		void visit(visitor* vst) override;
	};

	struct string_literal final : literal
	{
		std::string value;

		explicit string_literal(utils::position_range range, std::string value) :
			literal(range), value(std::move(value))
		{}

		void visit(visitor* vst) override;
	};

	struct number_literal final : literal
	{
		std::variant<std::string,
			std::int8_t, std::int16_t, std::int32_t, std::int64_t,
			std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
			float, double> value;

		explicit number_literal(utils::position_range range, std::string value) :
			literal(range), value(std::move(value))
		{}

		void visit(visitor* vst) override;
	};

	struct unresolved_symbol final : expression
	{
		std::string name;

		explicit unresolved_symbol(const utils::position_range range, std::string variable_name)
			: expression(range), name(std::move(variable_name)) {}

		void visit(visitor* vst) override;
	};

	struct variable final : expression
	{
		std::unique_ptr<expression> value;

		explicit variable(const utils::position_range range, std::unique_ptr<expression> expr)
			: expression(range), value(std::move(expr)) {}

		void visit(visitor* vst) override;
	};

	struct call final : expression
	{
		std::unique_ptr<expression> function;
		expression_list arguments;

		explicit call(const utils::position_range range, std::unique_ptr<expression> function, expression_list arguments)
			: expression(range), function(std::move(function)), arguments(std::move(arguments)) {}

		void visit(visitor* vst) override;
	};
}