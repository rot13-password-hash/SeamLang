#include "types.hpp"
#include "../../ir/ast/visitor.hpp"
#include "../../utils/exception.hpp"

#include <iostream>

namespace seam::parser::passes
{
	std::shared_ptr<ir::ast::type> get_dominant_type(const std::shared_ptr<ir::ast::type>& a, const std::shared_ptr<ir::ast::type>& b)
	{
		const auto built_in_type_a = std::get_if<ir::ast::type::built_in_type>(&a->value);
		const auto built_in_type_b = std::get_if<ir::ast::type::built_in_type>(&b->value);

		if (built_in_type_a && built_in_type_b)
		{
			if (((*built_in_type_a >= ir::ast::type::built_in_type::i8 && *built_in_type_a <= ir::ast::type::built_in_type::i64)
				&& (*built_in_type_b >= ir::ast::type::built_in_type::i8 && *built_in_type_b <= ir::ast::type::built_in_type::i64))

				|| ((*built_in_type_a >= ir::ast::type::built_in_type::u8 && *built_in_type_a <= ir::ast::type::built_in_type::u64)
					&& (*built_in_type_b >= ir::ast::type::built_in_type::u8 && *built_in_type_b <= ir::ast::type::built_in_type::u64))

				|| ((*built_in_type_a >= ir::ast::type::built_in_type::f32 && *built_in_type_a <= ir::ast::type::built_in_type::f64))
					&& (*built_in_type_b >= ir::ast::type::built_in_type::f32 && *built_in_type_b <= ir::ast::type::built_in_type::f64))
			{
				return (built_in_type_a > built_in_type_b) ? a : b;
			}
			else
			{
				throw std::runtime_error("can't determine dominant type, probably a type error");
			}
		}
		else
		{
			throw std::runtime_error("class types are not supported");
		}
	}

	struct type_getter : ir::ast::visitor
	{
		std::shared_ptr<ir::ast::type> type;

		bool visit(ir::ast::expression::variable_ref* var) override
		{
			type = var->var->type_;
			return false;
		}

		bool visit(ir::ast::expression::number_literal* num) override
		{
			return false;
		}

		bool visit(ir::ast::expression::binary* binary) override
		{
			binary->left->visit(this);
			const auto left_type = type;

			binary->right->visit(this);
			const auto right_type = type;

			type = get_dominant_type(left_type, right_type);

			return false;
		}
	};

	std::shared_ptr<ir::ast::type> resolve_type(ir::ast::expression::expression* expr)
	{
		type_getter vst;
		expr->visit(&vst);
		return vst.type;
	}

	struct visitor : ir::ast::visitor
	{
		bool visit(ir::ast::statement::assignment* node) override
		{
			if (const auto var = dynamic_cast<ir::ast::expression::variable_ref*>(node->to.get()))
			{
				if (const auto built_in = std::get_if<ir::ast::type::built_in_type>(&var->var->type_->value))
				{
					if (*built_in == ir::ast::type::built_in_type::auto_)
					{
						var->var->type_ = resolve_type(node->from.get());
					}
				}
			}
			return false;
		}
	};

	void types::run(ir::ast::node* node)
	{
		visitor vst;
		node->visit(&vst);
	}
}
