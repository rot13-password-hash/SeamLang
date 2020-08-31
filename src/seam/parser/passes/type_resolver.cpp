#include "type_resolver.hpp"
#include "../../ir/ast/type.hpp"
#include "../../utils/exception.hpp"
#include "../../ir/ast/visitor.hpp"

#include <iostream>

namespace seam::parser::passes
{
    using namespace ir::ast;

	template<typename T>
	inline bool in_numeric_limits(T val)
	{
		return val >= std::numeric_limits<T>::min()
			&& val <= std::numeric_limits<T>::max();
	}

	struct resolver : visitor
	{
		const function_map& function_map_;
		const type_map& type_map_;

		std::unique_ptr<expression::resolved_number> resolve_float(seam::utils::position position, const std::string& number_string)
		{
			double number_f64 = std::stod(number_string);

			if (in_numeric_limits<float>(number_f64))
			{
				return std::make_unique<expression::resolved_number>(false, static_cast<float>(number_f64));
			}

			return std::make_unique<expression::resolved_number>(false, number_f64);
		}

		std::unique_ptr<expression::resolved_number> resolve_signed(seam::utils::position position, const std::string& number_string)
		{
			std::int64_t number_i64 = std::stoll(number_string);

			if (in_numeric_limits<std::int8_t>(number_i64))
			{
				return std::make_unique<expression::resolved_number>(false, static_cast<std::uint8_t>(number_i64));
			}

			if (in_numeric_limits<std::int16_t>(number_i64))
			{
				return std::make_unique<expression::resolved_number>(false, static_cast<std::uint16_t>(number_i64));
			}

			if (in_numeric_limits<std::int32_t>(number_i64))
			{
				return std::make_unique<expression::resolved_number>(false, static_cast<std::uint32_t>(number_i64));
			}

			return std::make_unique<expression::resolved_number>(false, static_cast<std::uint64_t>(number_i64));
		}

		std::unique_ptr<expression::resolved_number> resolve_unsigned(seam::utils::position position, const std::string& number_string)
		{
			std::uint64_t number_u64 = std::stoull(number_string);

			if (in_numeric_limits<std::uint8_t>(number_u64))
			{
				return std::make_unique<expression::resolved_number>(true, static_cast<std::uint8_t>(number_u64));
			}

			if (in_numeric_limits<std::uint16_t>(number_u64))
			{
				return std::make_unique<expression::resolved_number>(true, static_cast<std::uint16_t>(number_u64));
			}

			if (in_numeric_limits<std::uint32_t>(number_u64))
			{
				return std::make_unique<expression::resolved_number>(true, static_cast<std::uint32_t>(number_u64));
			}

			return std::make_unique<expression::resolved_number>(true, number_u64);
		}

		bool visit(expression::number_wrapper* node) override
		{
			const auto unresolved = static_cast<expression::unresolved_number*>(node->value.get());
			const auto& number_string = unresolved->value;
			if (number_string.find('.') != std::string::npos)
			{
				node->value = resolve_float(node->range.start, number_string);
			}
			else if (number_string.at(0) == '-')
			{
				node->value = resolve_signed(node->range.start, number_string);
			}
			else
			{
				node->value = resolve_unsigned(node->range.start, number_string);
			}

			return false;
		}

		bool visit(type_wrapper* node) override
		{
			const auto& it = type_map_.find(static_cast<unresolved_type*>(node->value.get())->name);
			if (it != type_map_.cend())
			{
				node->value = it->second;
			}
			else
			{
				throw utils::parser_exception{ node->range.start, "internal parser error: cannot resolve type" };
			}

			return false;
		}

		bool visit(expression::symbol_wrapper* node) override
		{
			const auto& it = function_map_.find(static_cast<expression::unresolved_symbol*>(node->value.get())->value);
			if (it == function_map_.cend())
			{
				throw utils::parser_exception{ node->range.start, "internal parser error: cannot resolve symbol" };
			}
			node->value = std::make_unique<expression::resolved_symbol>(it->second);
			
			return false;
		}

		resolver(const type_map& type_map_, const function_map& function_map_) :
			type_map_(type_map_), function_map_(function_map_)
		{}
	};

	void type_resolver::run(node* node)
	{
		resolver vst{ type_map_, function_map_ };
		node->visit(&vst);
	}

	type_resolver::type_resolver(const type_map& type_map_, const function_map& function_map_) :
		type_map_(type_map_), function_map_(function_map_)
	{}
}
