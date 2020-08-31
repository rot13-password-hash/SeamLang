#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "node.hpp"
#include "expression.hpp"
#include "type.hpp"

namespace seam::ir::ast::statement
{	
	struct statement : node
	{
		explicit statement(utils::position_range range)
			: node(range) {}

		virtual ~statement() = default;
	};

	using statement_list = std::vector<std::unique_ptr<statement>>;

	struct restricted : statement
	{
		explicit restricted(utils::position_range range)
			: statement(range) {}

		virtual ~restricted() = default;
	};

	using restricted_list = std::vector<std::unique_ptr<restricted>>;

	struct restricted_block final : statement
	{	
		restricted_list body;

		void visit(visitor* vst) override;

		explicit restricted_block(utils::position_range range, restricted_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct block final : statement
	{
		statement_list body;
		block* parent;
		std::unordered_map<std::string, std::shared_ptr<expression::variable>> variables;
		
		void visit(visitor* vst) override;

		explicit block(utils::position_range range)
			: statement(range) {}

		explicit block(utils::position_range range, statement_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct expression_ final : statement
	{
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit expression_(utils::position_range range, std::unique_ptr<expression::expression> value) :
			statement(range), value(std::move(value)) {}
	};

	struct ret final : statement
	{
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit ret(utils::position_range range, std::unique_ptr<expression::expression> return_value) :
			statement(range), value(std::move(return_value)) {}
	};

	struct variable_declaration final : statement
	{
		std::string name;
		std::unique_ptr<type_wrapper> type;
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit variable_declaration(utils::position_range range, std::string var_name, std::unique_ptr<type_wrapper> type, std::unique_ptr<expression::expression> value) :
			statement(range), name(var_name), type(std::move(type)), value(std::move(value)) {}
	};

	struct variable_assignment final : statement
	{
		std::string name;
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit variable_assignment(utils::position_range range, std::string var_name, std::unique_ptr<expression::expression> value) :
			statement(range), name(var_name), value(std::move(value)) {}
	};

	struct if_stat final : statement
	{
		std::unique_ptr<expression::expression> condition;
		std::unique_ptr<block> main_body;
		std::unique_ptr<block> else_body;
		// elseif

		void visit(visitor* vst) override;
		
		explicit if_stat(utils::position_range range,
			std::unique_ptr<expression::expression> condition,
			std::unique_ptr<block> main_body,
			std::unique_ptr<block> else_body) :
			statement(range),
			condition(std::move(condition)),
			main_body(std::move(main_body)),
			else_body(std::move(else_body)) {}
	};
	
	struct loop : statement
	{
		// numberial for loop, range for loop
		// initial, final, <optional step>
		std::unique_ptr<block> body;

		void visit(visitor* vst) override;

		explicit loop(utils::position_range range, std::unique_ptr<block> body) :
			statement(range), body(std::move(body)) {}
	};

	struct numerical_for_loop final : loop 
	{
		std::unique_ptr<expression::number_wrapper> initial; // used as variable
		std::unique_ptr<expression::number_wrapper> final;
		std::unique_ptr<expression::number_wrapper> step;

		void visit(visitor* vst) override;

		explicit numerical_for_loop(utils::position_range range, std::unique_ptr<expression::number_wrapper> initial,
			std::unique_ptr<expression::number_wrapper> final, std::unique_ptr<expression::number_wrapper> step, std::unique_ptr<block> body)
				: loop(range, std::move(body)), 
					initial(std::move(initial)), final(std::move(final)), step(std::move(step)) {}
	};
	
	struct while_loop final : loop
	{
		std::unique_ptr<expression::expression> condition;

		void visit(visitor* vst) override;

		explicit while_loop(utils::position_range range, std::unique_ptr<expression::expression> condition, std::unique_ptr<block> body) :
			loop(range, std::move(body)), condition(std::move(condition)) {}
	};

	struct function_definition final : restricted
	{
		std::shared_ptr<function_signature> signature;
		std::unique_ptr<block> body;
		std::unordered_set<std::shared_ptr<function_signature>> function_dependencies;

		void visit(visitor* vst) override;

		explicit function_definition(utils::position_range range, std::shared_ptr<function_signature> signature, std::unique_ptr<block> body) :
			restricted(range), signature(signature), body(std::move(body)) {}
	};

	struct type_definition : restricted
	{
		using restricted::restricted;
	};

	struct alias_type_definition final : type_definition
	{
		std::string name;
		std::unique_ptr<type_wrapper> target_type;

		void visit(visitor* vst) override;

		explicit alias_type_definition(utils::position_range range, std::string name, std::unique_ptr<type_wrapper> target_type) :
			type_definition(range), name(std::move(name)), target_type(std::move(target_type)) {}
	};

	struct class_type_definition final : type_definition
	{
		std::string name;
		parameter_list fields;
		std::unique_ptr<restricted_block> body;

		void visit(visitor* vst) override;

		explicit class_type_definition(utils::position_range range, std::string name, parameter_list fields,
			std::unique_ptr<restricted_block> body) :
			type_definition(range),
			name(std::move(name)),
			fields(std::move(fields)),
			body(std::move(body)) {}
	};
}
