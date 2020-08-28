#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "node.hpp"
#include "type.hpp"
#include "../ast/expression.hpp"
namespace seam::ir::ast
{
	struct expression;
	struct statement : node
	{
		explicit statement(const utils::position_range range)
			: node(range) {}

		virtual ~statement() = default;
	};

	using statement_list = std::vector<std::unique_ptr<statement>>;

	struct restricted_statement : statement
	{
		explicit restricted_statement(const utils::position_range range)
			: statement(range) {}

		virtual ~restricted_statement() = default;
	};

	using restricted_statement_list = std::vector<std::unique_ptr<restricted_statement>>;

	struct restricted_block final : statement
	{	
		restricted_statement_list body;

		void visit(base_visitor* vst) override;
		void visit_children(base_visitor* vst);

		explicit restricted_block(const utils::position_range range, restricted_statement_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct block final : statement
	{
		statement_list body;
		
		void visit(base_visitor* vst) override;
		void visit_children(base_visitor* vst);

		explicit block(const utils::position_range range, statement_list body)
			: statement(range), body(std::move(body)) {}
	};

	struct return_statement final : statement
	{
		std::unique_ptr<expression> value;

		void visit(base_visitor* vst) override;
		void visit_children(base_visitor* vst);

		explicit return_statement(utils::position_range range, std::unique_ptr<expression> return_value) :
			statement(range), value(std::move(return_value)) {}
	};
	
	struct parameter final
	{		
		type param_type;
		std::string name;

		explicit parameter(type param_type, std::string name) :
			param_type(std::move(param_type)), name(std::move(name)) {}
	};

	using parameter_list = std::vector<parameter>;
	using attribute_list = std::unordered_set<std::string>;
	
	struct function_signature final
	{
		std::string name;
		type return_type;
		std::vector<parameter> parameters;
		std::unordered_set<std::string> attributes;

		explicit function_signature(std::string name, type return_type, parameter_list parameters,
			attribute_list attributes) :
			name(std::move(name)),
			return_type(std::move(return_type)),
			parameters(std::move(parameters)),
			attributes(std::move(attributes)) {}
	};

	struct function_definition final : restricted_statement
	{
		std::unique_ptr<function_signature> signature;
		std::unique_ptr<block> body;

		void visit(base_visitor* vst) override;
		void visit_children(base_visitor* vst);

		explicit function_definition(const utils::position_range range, std::unique_ptr<function_signature> signature, std::unique_ptr<block> body) :
			restricted_statement(range), signature(std::move(signature)), body(std::move(body)) {}
	
	};

	struct extern_function_definition final : restricted_statement
	{
		function_signature signature;

		void visit(base_visitor* vst) override;

		explicit extern_function_definition(function_signature signature) :
			restricted_statement(range), signature(signature) {}
	};

	struct type_definition : restricted_statement
	{
		using restricted_statement::restricted_statement;
	};

	struct alias_type_definition final : type_definition
	{
		std::string name;
		type target_type;

		void visit(base_visitor* vst) override;

		explicit alias_type_definition(const utils::position_range range, std::string name, type target_type) :
			type_definition(range), name(std::move(name)), target_type(std::move(target_type)) {}
	};

	struct class_type_definition final : type_definition
	{
		std::string name;
		parameter_list fields;
		std::unique_ptr<restricted_block> body;

		void visit(base_visitor* vst) override;
		void visit_children(base_visitor* vst);

		explicit class_type_definition(const utils::position_range range, std::string name, parameter_list fields,
			std::unique_ptr<restricted_block> body) :
			type_definition(range),
			name(std::move(name)),
			fields(std::move(fields)),
			body(std::move(body)) {}
	};
}
