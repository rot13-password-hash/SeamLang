#pragma once

#include "node.hpp"
#include "base_visitor.hpp"

namespace seam::ir::ast
{	
	struct expression : node
	{
		explicit expression(const utils::position_range range)
			: node(range) {}

		virtual ~expression() = default;
	};

	using expression_list = std::vector<std::unique_ptr<expression>>;

	template <typename T>
	struct literal final : expression
	{		
		T value;

		explicit literal(const utils::position_range range, T value)
			: expression(range), value(std::move(value)) {}
		
		void visit(base_visitor* vst) override
		{
			vst->base_visit(this);
		}
	};

	struct unresolved_symbol final : expression
	{		
		std::string name;

		explicit unresolved_symbol(const utils::position_range range, std::string variable_name)
			: expression(range), name(std::move(variable_name)) {}

		void visit(base_visitor* vst) override;
	};

	struct variable final : expression
	{		
		std::unique_ptr<expression> value;

		explicit variable(const utils::position_range range, std::unique_ptr<expression> expr)
			: expression(range), value(std::move(expr)) {}
		
		void visit(base_visitor* vst) override;
	protected:
		void visit_children(base_visitor* vst);
	};

	struct call_expression final : expression
	{
		std::unique_ptr<expression> function;
		expression_list arguments;

		explicit call_expression(const utils::position_range range, std::unique_ptr<expression> function, expression_list arguments)
			: expression(range), function(std::move(function)), arguments(std::move(arguments)) {}

		void visit(base_visitor* vst) override;
	protected:
		void visit_children(base_visitor* vst);
	};
}