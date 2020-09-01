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
		std::shared_ptr<type> eval_type{};

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

	struct variable
	{
		std::string name;
		std::shared_ptr<type> type_;

		variable(std::string name, std::shared_ptr<type> type_) :
			name(std::move(name)),
			type_(std::move(type_))
		{}
	};

	struct variable_ref final : expression
	{
		std::shared_ptr<variable> var;

		void visit(visitor* vst) override;

		variable_ref(utils::position_range range, std::shared_ptr<variable> var) :
			expression(range),
			var(std::move(var))
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

		void visit(visitor* vst) override;

		explicit bool_literal(utils::position_range range, bool value) :
			literal(range), value(value)
		{}
	};

	struct string_literal : literal
	{
		std::string value;

		void visit(visitor* vst) override;

		explicit string_literal(utils::position_range range, std::string value) :
			literal(range), value(std::move(value))
		{}
	};

	struct number_literal final : literal
	{
		std::variant<std::uint64_t, double> value;
		bool is_unsigned;

		void visit(visitor* vst) override;

		void parse_float(const std::string& value);
		void parse_integer(const std::string& value, bool is_unsigned);

		explicit number_literal(utils::position_range range, const std::string& value);
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

	using parameter = variable_ref;
	using parameter_list = std::vector<std::unique_ptr<parameter>>;
	using attribute_list = std::unordered_set<std::string>;

	struct function_signature : node
	{
		std::string name;
		std::shared_ptr<type> return_type;
		std::vector<std::unique_ptr<variable_ref>> parameters;
		std::unordered_set<std::string> attributes;
		bool is_extern = false;

		std::string mangled_name;

		explicit function_signature(std::string module_name, std::string name, std::shared_ptr<type> return_type, parameter_list parameters,
			attribute_list attributes) :
			node({ 0,0 }),
			name(std::move(name)),
			return_type(std::move(return_type)),
			parameters(std::move(parameters)),
			attributes(std::move(attributes))
		{
			mangled_name = module_name + "@" + this->name;
		}

		void visit(visitor* vst) override;
	};

	struct resolved_symbol final : symbol
	{
		std::shared_ptr<function_signature> signature;

		explicit resolved_symbol(std::shared_ptr<function_signature> sig) :
			signature(std::move(sig))
		{}
	};
}