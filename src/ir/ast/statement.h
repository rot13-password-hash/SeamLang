#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "node.h"
#include "type.h"

namespace seam::ir::ast
{
	/**
	 * Base statement node.
	 */
	struct statement : node
	{
		explicit statement(const utils::position_range range)
			: node(range) {}

		virtual ~statement() = default;
	};

	using statement_list = std::vector<std::unique_ptr<statement>>;

	/**
	 * Restricted statement node.
	 *
	 * A restricted statement is any statement which is in a
	 * restricted block. The following are considered restricted
	 * statements:
	 *
	 * - Function definitions
	 * - Type definitions
	 * - Extern definitions
	 */
	struct restricted_statement : statement
	{
		explicit restricted_statement(const utils::position_range range)
			: statement(range) {}

		virtual ~restricted_statement() = default;
	};

	using restricted_statement_list = std::vector<std::unique_ptr<restricted_statement>>;

	/**
	 * Restricted block node.
	 *
	 * A restricted block is considered to be a top-level block
	 * which can consist of restricted statements.
	 */
	struct restricted_block final : statement
	{
		explicit restricted_block(const utils::position_range range, restricted_statement_list body)
			: statement(range), body(std::move(body)) {}
		
		restricted_statement_list body;

		void visit(visitor* vst) override;
	private:
		void visit_children(visitor* vst);
	};

	/**
	 * Block node.
	 *
	 * Generic block.
	 * TODO: Explain more.
	 */
	struct block final : statement
	{
		explicit block(const utils::position_range range, statement_list body)
			: statement(range), body(std::move(body)) {}
		
		statement_list body;

		void visit(visitor* vst) override;
	private:
		void visit_children(visitor* vst);
	};
	
	/**
	 * TODO: Explain.
	 */
	struct parameter
	{
		type param_type;
		std::string name;

		parameter(type param_type, std::string name)
			: param_type(std::move(param_type)), name(std::move(name)) {}
	};

	using parameter_list = std::vector<parameter>;
	using attribute_list = std::unordered_set<std::string>;
	
	/**
	 * TODO: Explain.
	 */
	struct function_declaration : restricted_statement
	{
		explicit function_declaration(utils::position_range range, std::string name, type return_type, parameter_list parameters,
			attribute_list attributes)
			:   restricted_statement(range),
				name(std::move(name)),
				return_type(std::move(return_type)),
				parameters(std::move(parameters)),
				attributes(std::move(attributes)) {}

		virtual ~function_declaration() = default;

		std::string name;
		type return_type;
		std::vector<parameter> parameters;
		std::unordered_set<std::string> attributes;		
	};

	/**
	 * TODO: Explain.
	 */
	struct function_definition final : function_declaration
	{
		explicit function_definition(const utils::position_range range, std::string name, type return_type, parameter_list parameters,
			attribute_list attributes, std::unique_ptr<block> func_body)
				: function_declaration(range, std::move(name), std::move(return_type),
								std::move(parameters), std::move(attributes)), body(std::move(func_body)) {}

		std::unique_ptr<block> body;

		void visit(visitor* vst) override;
	private:
		void visit_children(visitor* vst);
	};

	struct type_definition : restricted_statement
	{
		using restricted_statement::restricted_statement;
	};

	struct alias_type_definition final : type_definition
	{
		explicit alias_type_definition(const utils::position_range range, std::string name, type target_type)
				: type_definition(range), name(std::move(name)), target_type(std::move(target_type)) {}

		std::string name;
		type target_type;

		void visit(visitor* vst) override;
	};

	struct class_type_definition final : type_definition
	{
		explicit class_type_definition(const utils::position_range range, std::string name, parameter_list fields,
			std::unique_ptr<restricted_block> body)
			: type_definition(range), name(std::move(name)), fields(std::move(fields)), body(std::move(body)) {}
		
		std::string name;
		parameter_list fields;
		std::unique_ptr<restricted_block> body;

		void visit(visitor* vst) override;
	private:
		void visit_children(visitor* vst);
	};
}
