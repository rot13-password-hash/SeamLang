#pragma once

#include <vector>
#include <memory>
#include <string>
#include <variant>

#include "node.hpp"
#include "type.hpp"
#include "../../lexer/lexeme.hpp"

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

	struct unary : expression
	{
		std::unique_ptr<expression> right;
		lexer::lexeme_type operation;

		void visit(visitor* vst) override;

		explicit unary(utils::position_range range,
			std::unique_ptr<expression> rhs,
			lexer::lexeme_type operation) :
			expression(range),
			right(std::move(rhs)),
			operation(operation)
		{}
	};

	struct binary : expression
	{
		std::unique_ptr<expression> left;
		std::unique_ptr<expression> right;
		lexer::lexeme_type operation;

		void visit(visitor* vst) override;
		
		explicit binary(utils::position_range range,
			std::unique_ptr<expression> lhs,
			std::unique_ptr<expression> rhs,
			lexer::lexeme_type operation) :
			expression(range),
			left(std::move(lhs)),
			right(std::move(rhs)),
			operation(operation)
		{}
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
		std::unique_ptr<number> value;

		void visit(visitor* vst) override;

		number_wrapper(utils::position_range range, std::unique_ptr<number> value) :
			literal(range), value(std::move(value))
		{}
	};

	struct unresolved_number final : number
	{
		std::string value;

		unresolved_number(std::string value) :
			value(std::move(value))
		{}
	};

	struct resolved_number final : number
	{
		bool is_unsigned;
		std::variant<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, float, double> value;

		resolved_number(bool is_unsigned, std::variant<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t, float, double> value) :
			is_unsigned(is_unsigned),
			value(std::move(value))
		{}
	};

	struct call final : expression
	{
		std::unique_ptr<expression> function;
		expression_list arguments;

		explicit call(const utils::position_range range, std::unique_ptr<expression> function, expression_list arguments) :
			expression(range), function(std::move(function)), arguments(std::move(arguments))
		{}

		void visit(visitor* vst) override;
	};

	struct symbol
	{
		
	};
	
	struct symbol_wrapper final : expression
	{
		std::unique_ptr<symbol> value;

		void visit(visitor* vst) override;

		symbol_wrapper(utils::position_range range, std::unique_ptr <symbol> value) :
			expression(range), value(std::move(value))
		{}
	};

	struct unresolved_symbol final : symbol
	{
		std::string value;

		explicit unresolved_symbol(std::string value) :
			value(std::move(value))
		{}
	};

	struct resolved_symbol final : symbol
	{
		std::shared_ptr<function_signature> signature;

		explicit resolved_symbol(std::shared_ptr<function_signature> sig) :
			signature(std::move(sig))
		{}
	};
}