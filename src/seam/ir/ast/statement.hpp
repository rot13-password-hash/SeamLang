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

	struct base_block : statement
	{
		base_block* parent;
		std::unordered_map<std::string, std::shared_ptr<expression::variable>> variables;
		std::unordered_map<std::string, std::shared_ptr<type>> types;

		explicit base_block(utils::position_range range) :
			statement(range)
		{}
	};

	struct restricted_block final : base_block
	{	
		restricted_list body;

		void visit(visitor* vst) override;

		explicit restricted_block(utils::position_range range)
			: base_block(range)
		{}

		explicit restricted_block(utils::position_range range, restricted_list body)
			: base_block(range), body(std::move(body))
		{}
	};

	struct normal_block final : base_block
	{
		statement_list body;
		
		void visit(visitor* vst) override;

		explicit normal_block(utils::position_range range)
			: base_block(range) {}

		explicit normal_block(utils::position_range range, statement_list body)
			: base_block(range), body(std::move(body)) {}
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

	struct assignment final : statement
	{
		std::unique_ptr<expression::expression> to;
		std::unique_ptr<expression::expression> from;

		void visit(visitor* vst) override;

		explicit assignment(utils::position_range range, std::unique_ptr<expression::expression> to, std::unique_ptr<expression::expression> from) :
			statement(range),
			to(std::move(to)),
			from(std::move(from))
		{}
	};

	struct if_stat final : statement
	{
		std::unique_ptr<expression::expression> condition;
		std::unique_ptr<normal_block> main_body;
		std::unique_ptr<normal_block> else_body;
		// elseif

		void visit(visitor* vst) override;
		
		explicit if_stat(utils::position_range range,
			std::unique_ptr<expression::expression> condition,
			std::unique_ptr<normal_block> main_body,
			std::unique_ptr<normal_block> else_body) :
			statement(range),
			condition(std::move(condition)),
			main_body(std::move(main_body)),
			else_body(std::move(else_body)) {}
	};
	
	struct loop : statement
	{
		// numberial for loop, range for loop
		// initial, final, <optional step>
		std::unique_ptr<normal_block> body;

		void visit(visitor* vst) override;

		explicit loop(utils::position_range range, std::unique_ptr<normal_block> body) :
			statement(range), body(std::move(body)) {}
	};

	struct numerical_for_loop final : loop 
	{
		std::unique_ptr<expression::number_literal> initial; // used as variable
		std::unique_ptr<expression::number_literal> final;
		std::unique_ptr<expression::number_literal> step;

		void visit(visitor* vst) override;

		explicit numerical_for_loop(utils::position_range range, std::unique_ptr<expression::number_literal> initial,
			std::unique_ptr<expression::number_literal> final, std::unique_ptr<expression::number_literal> step, std::unique_ptr<normal_block> body)
				: loop(range, std::move(body)), 
					initial(std::move(initial)), final(std::move(final)), step(std::move(step)) {}
	};
	
	struct while_loop final : loop
	{
		std::unique_ptr<expression::expression> condition;

		void visit(visitor* vst) override;

		explicit while_loop(utils::position_range range, std::unique_ptr<expression::expression> condition, std::unique_ptr<normal_block> body) :
			loop(range, std::move(body)), condition(std::move(condition)) {}
	};

	struct function_definition final : restricted
	{
		std::shared_ptr<expression::function_signature> signature;
		std::unique_ptr<normal_block> body;
		std::unordered_set<std::shared_ptr<expression::function_signature>> function_dependencies;

		void visit(visitor* vst) override;

		explicit function_definition(utils::position_range range, std::shared_ptr<expression::function_signature> signature, std::unique_ptr<normal_block> body) :
			restricted(range), signature(signature), body(std::move(body)) {}
	};

	struct extern_function_definition final : restricted
	{
		std::shared_ptr<expression::function_signature> signature;

		void visit(visitor* vst);

		explicit extern_function_definition(utils::position_range range, std::shared_ptr<expression::function_signature> signature) :
			restricted(range),
			signature(signature)
		{}
	};

	struct type_definition : restricted
	{
		using restricted::restricted;
	};

	struct alias_type_definition final : type_definition
	{
		std::string name;
		std::shared_ptr<type> target_type;

		void visit(visitor* vst) override;

		explicit alias_type_definition(utils::position_range range, std::string name, std::shared_ptr<type> target_type) :
			type_definition(range), name(std::move(name)), target_type(std::move(target_type)) {}
	};

	struct class_type_definition final : type_definition
	{
		std::string name;
		expression::parameter_list fields;
		std::unique_ptr<restricted_block> body;

		void visit(visitor* vst) override;

		explicit class_type_definition(utils::position_range range, std::string name, expression::parameter_list fields,
			std::unique_ptr<restricted_block> body) :
			type_definition(range),
			name(std::move(name)),
			fields(std::move(fields)),
			body(std::move(body)) {}
	};
}
