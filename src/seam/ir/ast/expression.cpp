#include "expression.hpp"
#include "visitor.hpp"
#include <iostream>
namespace seam::ir::ast::expression
{
	void unary::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			right->visit(vst);
		}
	}

	void binary::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			left->visit(vst);
			right->visit(vst);
		}
	}

	void variable_ref::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void call::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			function->visit(vst);
			for (const auto& argument : arguments)
			{
				argument->visit(vst);
			}
		}
	}

	void bool_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void string_literal::visit(visitor* vst)
	{
		vst->visit(this);
	}

    void number_literal::visit(visitor* vst)
    {
		vst->visit(this);
    }

	void number_literal::parse_float(const std::string& value)
	{
		this->value = std::stod(value);
	}

	void number_literal::parse_integer(const std::string& value, bool is_unsigned)
	{
		this->value = std::stoull(value.c_str());
		this->is_unsigned = is_unsigned;
	}

	number_literal::number_literal(utils::position_range range, const std::string& value) :
		literal(range)
	{
		if (value.find('.') != std::string::npos)
		{
			parse_float(value);
		}
		else
		{
			parse_integer(value, value[0] != '-');
		}
	}
	
	void symbol_wrapper::visit(visitor* vst)
	{
		vst->visit(this);
	}

	void function_signature::visit(visitor* vst)
	{
		if (vst->visit(this))
		{
			for (const auto& param : parameters)
			{
				param->visit(vst);
			}
		}
	}
}