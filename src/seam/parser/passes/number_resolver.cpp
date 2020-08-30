#include "number_resolver.hpp"

#include "../../ir/ast/visitor.hpp"
#include "../../utils/exception.hpp"
#include <iostream>

namespace seam::parser::passes
{
	using namespace ir::ast;
	using built_in_type = resolved_type::built_in_type;

	struct resolver_visitor : visitor
	{
		built_in_type number_type = built_in_type::void_;

		static built_in_type get_builtin_type(resolved_type* type)
		{
			if (type->is_auto)
			{
				throw std::runtime_error{ "TODO: add support for auto" };
			}

			return std::visit(
				[](auto&& value) -> built_in_type
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(value)>, built_in_type>)
					{
						return value;
					}
					return built_in_type::void_;
				}, type->value);
		}

		bool visit(expression::number_literal* node)
		{
			auto& number_string = std::get<std::string>(node->value);
			if (number_string.find('.') != std::string::npos)
			{
				// floating point
				double number_f64;
				try
				{
					number_f64 = std::stod(number_string);
				}
				catch (const std::out_of_range& ex)
				{
					throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
				}

				switch (number_type)
				{
				case built_in_type::f32:
				{
					// TODO: better error? since if the number is negative, "too large" may be misleading.
					if (number_f64 < std::numeric_limits<float>::min()
						|| number_f64 > std::numeric_limits<float>::max())
					{
						throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
					}
					node->value = static_cast<float>(number_f64);
					break;
				}
				case built_in_type::f64:
				{
					node->value = number_f64;
					break;
				}
				default:
				{
					// TODO: better error message?
					throw utils::parser_exception{ node->range.start, "number '" + number_string + "' is a floating-point number" };
				}
				}
			}
			else
			{
				// integer
				switch (number_type)
				{
				case built_in_type::i8:
				case built_in_type::i16:
				case built_in_type::i32:
				case built_in_type::i64:
				case built_in_type::f32:
				case built_in_type::f64:
				{
					std::int64_t number_i64;
					try
					{
						number_i64 = std::stoll(number_string);
					}
					catch (const std::out_of_range& ex)
					{
						throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
					}

					switch (number_type)
					{
					case built_in_type::i8:
					{
						if (number_i64 < std::numeric_limits<std::int8_t>::min()
							|| number_i64 > std::numeric_limits<std::int8_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::int8_t>(number_i64);
						break;
					}
					case built_in_type::i16:
					{
						if (number_i64 < std::numeric_limits<std::int16_t>::min()
							|| number_i64 > std::numeric_limits<std::int16_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::int16_t>(number_i64);
						break;
					}
					case built_in_type::i32:
					{
						if (number_i64 < std::numeric_limits<std::int32_t>::min()
							|| number_i64 > std::numeric_limits<std::int32_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::int32_t>(number_i64);
						break;
					}
					case built_in_type::i64:
					{
						node->value = number_i64;
						break;
					}
					case built_in_type::f32:
					{
						if (number_i64 < std::numeric_limits<float>::min()
							|| number_i64 > std::numeric_limits<float>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<float>(number_i64);
						break;
					}
					case built_in_type::f64:
					{
						if (number_i64 < std::numeric_limits<double>::min()
							|| number_i64 > std::numeric_limits<double>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<double>(number_i64);
						break;
					}
					default:
					{
						// TODO: better error message?
						throw utils::parser_exception{ node->range.start, "number '" + number_string + "' is a signed integer" };
					}
					}
					break;
				}
				case built_in_type::u8:
				case built_in_type::u16:
				case built_in_type::u32:
				case built_in_type::u64:
				{
					// stoull accepts negative numbers for some reason
					if (number_string.at(0) != '-')
					{
						throw utils::parser_exception{ node->range.start, "unsigned number must not be negative" };
					}

					std::uint64_t number_u64;
					try
					{
						number_u64 = std::stoull(number_string);
					}
					catch (const std::out_of_range& ex)
					{
						throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
					}

					switch (number_type)
					{
					case built_in_type::u8:
					{
						if (number_u64 > std::numeric_limits<std::uint8_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::uint8_t>(number_u64);
						break;
					}
					case built_in_type::u16:
					{
						if (number_u64 > std::numeric_limits<std::uint16_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::uint16_t>(number_u64);
						break;
					}
					case built_in_type::u32:
					{
						if (number_u64 > std::numeric_limits<std::uint32_t>::max())
						{
							throw utils::parser_exception{ node->range.start, "number '" + number_string + "' too large" };
						}
						node->value = static_cast<std::uint32_t>(number_u64);
						break;
					}
					case built_in_type::u64:
					{
						node->value = number_u64;
						break;
					}
					default:
					{
						// TODO: better error message?
						throw utils::parser_exception{ node->range.start, "number '" + number_string + "' is an unsigned integer" };
					}
					}
					break;
				}
				default:
				{
					// TODO: better error message?
					throw utils::parser_exception{ node->range.start, "number '" + number_string + "' is an integer" };
				}
				}
			}
		}

		bool visit(statement::function_definition* node)
		{
			node->signature->visit(this);

			auto old_number_type = number_type;
			number_type = get_builtin_type(static_cast<resolved_type*>(node->signature->return_type->value.get()));

			node->body->visit(this);

			number_type = old_number_type;
			return false;
		}

		bool visit(statement::variable_declaration* node)
		{
			auto old_number_type = number_type;
			number_type = get_builtin_type(static_cast<resolved_type*>(node->type->value.get()));

			node->value->visit(this);

			number_type = old_number_type;
			return false;
		}

		bool visit(statement::variable_assignment* node)
		{
			throw utils::parser_exception{ node->range.start, "TODO: track variable type" };
		}
	};

	void number_resolver::run(ir::ast::node* node)
	{
		resolver_visitor vst;
		node->visit(&vst);
	}

	number_resolver::number_resolver()
	{
	}
}
