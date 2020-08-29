#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "node.hpp"
#include "../../types/function_signature.hpp"
#include "../ast/expression.hpp"

namespace seam::ir::ast::expression
{
	struct expression;
}

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

	struct function_definition final : restricted
	{
		types::function_signature signature;
		std::unique_ptr<block> body;

		void visit(visitor* vst) override;

		explicit function_definition(const utils::position_range range, types::function_signature signature, std::unique_ptr<block> body) :
			restricted(range), signature(std::move(signature)), body(std::move(body)) {}
	
	};

	struct extern_function_definition final : restricted
	{
		types::function_signature signature;

		void visit(visitor* vst) override;

		explicit extern_function_definition(const utils::position_range range, types::function_signature signature) :
			restricted(range), signature(std::move(signature)) {}
	};

	struct type_definition : restricted
	{
		using restricted::restricted;
	};

	struct alias_type_definition final : type_definition
	{
		std::string name;
		std::unique_ptr<unresolved_type> target_type;

		void visit(visitor* vst) override;

		explicit alias_type_definition(const utils::position_range range, std::string name, std::unique_ptr<unresolved_type> target_type) :
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
