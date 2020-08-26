#pragma once

#include <memory>
#include <string>
#include <unordered_set>
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

	/**
	 * Restricted block node.
	 *
	 * A restricted block is considered to be a top-level block
	 * which can consist of restricted statements.
	 */
	struct restricted_block final : statement
	{
		explicit restricted_block(const utils::position_range range, std::vector<std::unique_ptr<restricted_statement>> body)
			: statement(range), body(std::move(body)) {}
		
		std::vector<std::unique_ptr<restricted_statement>> body;

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
		explicit block(const utils::position_range range, std::vector<std::unique_ptr<statement>> body)
			: statement(range), body(std::move(body)) {}
		
		std::vector<std::unique_ptr<statement>> body;

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

	/**
	 * TODO: Explain.
	 */
	struct function_declaration final : restricted_statement
	{
		explicit function_declaration(utils::position_range range, std::string name, type return_type, std::vector<parameter> parameters,
			std::unordered_set<std::string> attributes)
			:   restricted_statement(range),
				name(std::move(name)),
				return_type(std::move(return_type)),
				parameters(std::move(parameters)),
				attributes(std::move(attributes)) {}
		
		std::string name;
		type return_type;
		std::vector<parameter> parameters;
		std::unordered_set<std::string> attributes;

		void visit(visitor* vst) override;
	private:
		void visit_children(visitor* vst);
	};
}
