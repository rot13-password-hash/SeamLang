#pragma once

#include <memory>
#include <string>
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
		explicit statement(const utils::position_range range)
			: node(range) {}

		virtual ~statement() = default;
	};

	using statement_list = std::vector<std::unique_ptr<statement>>;

	struct restricted : statement
	{
		explicit restricted(const utils::position_range range)
			: statement(range) {}

		virtual ~restricted() = default;
	};

	using restricted_list = std::vector<std::unique_ptr<restricted>>;

	struct restricted_block final : statement
	{	
		restricted_list body;

		void visit(visitor* vst) override;

		explicit restricted_block(const utils::position_range range, restricted_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct block final : statement
	{
		statement_list body;
		
		void visit(visitor* vst) override;

		explicit block(const utils::position_range range, statement_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct ret final : statement
	{
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit ret(const utils::position_range range, std::unique_ptr<expression::expression> return_value) :
			statement(range), value(std::move(return_value)) {}
	};

	struct variable_declaration final : statement
	{
		std::string name;
		std::unique_ptr<type_wrapper> type;
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit variable_declaration(const utils::position_range range, std::string var_name, std::unique_ptr<type_wrapper> type, std::unique_ptr<expression::expression> value) :
			statement(range), name(var_name), type(std::move(type)), value(std::move(value)) {}
	};

	struct variable_assignment final : statement
	{
		std::string name;
		std::unique_ptr<expression::expression> value;

		void visit(visitor* vst) override;

		explicit variable_assignment(const utils::position_range range, std::string var_name, std::unique_ptr<expression::expression> value) :
			statement(range), name(var_name), value(std::move(value)) {}
	};
	
	struct function_definition final : restricted
	{
		std::shared_ptr<function_signature> signature;
		std::unique_ptr<block> body;
		std::unordered_set<std::shared_ptr<function_signature>> function_dependencies;

		void visit(visitor* vst) override;

		explicit function_definition(const utils::position_range range, std::shared_ptr<function_signature> signature, std::unique_ptr<block> body) :
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

		explicit alias_type_definition(const utils::position_range range, std::string name, std::unique_ptr<type_wrapper> target_type) :
			type_definition(range), name(std::move(name)), target_type(std::move(target_type)) {}
	};

	struct class_type_definition final : type_definition
	{
		std::string name;
		parameter_list fields;
		std::unique_ptr<restricted_block> body;

		void visit(visitor* vst) override;

		explicit class_type_definition(const utils::position_range range, std::string name, parameter_list fields,
			std::unique_ptr<restricted_block> body) :
			type_definition(range),
			name(std::move(name)),
			fields(std::move(fields)),
			body(std::move(body)) {}
	};
}
