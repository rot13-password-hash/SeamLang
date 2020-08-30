#pragma once

#include <vector>
#include <memory>
#include <string>
#include <variant>

#include "node.hpp"
#include "type.hpp"

namespace seam::ir::ast
{
	struct visitor;
}

namespace seam::ir::ast::expression
{	
	struct expression : node
	{
		explicit expression(const utils::position_range range)
			: node(range)
		{}

		virtual ~expression() = default;
	};

	using expression_list = std::vector<std::unique_ptr<expression>>;

	struct variable final : expression
	{
		std::string name;
		std::shared_ptr<type_wrapper> type_;

		void visit(visitor* vst) override;

		variable(utils::position_range range, std::string name) :
			expression(range),
			name(std::move(name))
		{}
	};

	struct literal : expression
	{		
		void visit(visitor* vst) override = 0;

		explicit literal(utils::position_range range) :
			expression(range)
		{}
	};

	struct bool_literal : literal
	{
		bool value;

		explicit bool_literal(utils::position_range range, bool value) :
			literal(range), value(value)
		{}

		void visit(visitor* vst) override;
	};

	struct string_literal : literal
	{
		std::string value;

		explicit string_literal(utils::position_range range, std::string value) :
			literal(range), value(std::move(value))
		{}

		void visit(visitor* vst) override;
	};

	struct number
	{

	};

	struct number_wrapper final : literal
	{
		std::shared_ptr<number> value;

		void visit(visitor* vst) override;
		
		number_wrapper(utils::position_range range, std::shared_ptr<number> value) :
			literal(range), value(std::move(value))
		{}
	};

	struct unresolved_number final : number
	{
		std::string value;
	};

	struct resolved_number final : number
	{
		bool is_unsigned;
		std::variant<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, float, double> value;
	};

	struct number_literal : literal
	{
		std::string value;

		explicit number_literal(utils::position_range range, std::string value) :
			literal(range), value(std::move(value))
		{}

		void visit(visitor* vst) override;
	};

	struct call final : expression
	{
		std::unique_ptr<expression> function;
		expression_list arguments;

		explicit call(const utils::position_range range, std::unique_ptr<expression> function, expression_list arguments)
			: expression(range), function(std::move(function)), arguments(std::move(arguments))
		{}

		void visit(visitor* vst) override;
	};
}