#pragma once

#include "../utils/position.hpp"

#include <string>
#include <string_view>

namespace seam::lexer
{
	enum class lexeme_type
	{
		eof,
		identifier,
		attribute,
		
		// literals
		literal_string,
		literal_number,
		
		// symbols
		symbol_divide,
		symbol_divide_assign,
		symbol_add,
		symbol_add_assign,
		symbol_minus,
		symbol_minus_assign,
		symbol_multiply,
		symbol_multiply_assign,
		symbol_open_parenthesis,
		symbol_close_parenthesis,
		symbol_open_bracket,
		symbol_close_bracket,
		symbol_open_brace,
		symbol_close_brace,
		symbol_arrow,
		symbol_equals,
		symbol_declare,
		symbol_question_mark,
		symbol_colon,
		symbol_comma,
		
		// keywords
		kw_fn,
		kw_extern,
		kw_as,
		kw_return,
		kw_type,
		kw_try,
		kw_catch,
		kw_switch,
		kw_throw,
		kw_true,
		kw_false,
	};
	
	struct lexeme
	{
		lexeme_type type = lexeme_type::eof;
		std::string_view value {};
		utils::position position { 0, 0 };

		/**
		 * Returns string which corresponds with type.
		 *
		 * @param type lexeme type to convert to corresponding string.
		 * @returns type as a string.
		 */
		static std::string to_string(const lexeme_type type)
		{
			switch (type)
			{
				case lexeme_type::eof: return "<eof>";
				case lexeme_type::identifier: return "<identifier>";
				case lexeme_type::literal_string: return "<string literal>";
				case lexeme_type::literal_number: return "<number literal>";
				case lexeme_type::symbol_divide: return "/";
				case lexeme_type::symbol_divide_assign: return "/=";
				case lexeme_type::symbol_add: return "+";
				case lexeme_type::symbol_add_assign: return "+=";
				case lexeme_type::symbol_minus: return "-";
				case lexeme_type::symbol_minus_assign: return "-=";
				case lexeme_type::symbol_multiply: return "*";
				case lexeme_type::symbol_multiply_assign: return "*=";
				case lexeme_type::symbol_open_parenthesis: return "(";
				case lexeme_type::symbol_close_parenthesis: return ")";
				case lexeme_type::symbol_open_bracket: return "[";
				case lexeme_type::symbol_close_bracket: return "]";
				case lexeme_type::symbol_open_brace: return "{";
				case lexeme_type::symbol_close_brace: return "}";
				case lexeme_type::symbol_arrow: return "->";
				case lexeme_type::symbol_colon: return "=";
				case lexeme_type::symbol_comma: return ",";
				case lexeme_type::kw_fn: return "fn";
				case lexeme_type::kw_extern: return "extern";
				case lexeme_type::kw_as: return "as";
				case lexeme_type::kw_return: return "return";
				case lexeme_type::kw_type: return "type";
				case lexeme_type::kw_try: return "try";
				case lexeme_type::kw_catch: return "catch";
				case lexeme_type::kw_switch: return "switch";
				case lexeme_type::kw_throw: return "throw";
				case lexeme_type::kw_true: return "true";
				case lexeme_type::kw_false: return "false";
				default: return "<unknown>";
			}
		}

		/**
		 * Returns lexeme type as a string.
		 *
		 * @returns lexeme type as a string.
		 */
		[[nodiscard]] std::string to_string() const
		{
			switch (type)
			{
				case lexeme_type::literal_number:
				case lexeme_type::identifier:
				{
					return { value.cbegin(), value.cend() };
				}
				case lexeme_type::attribute:
				{
					return '@' + std::string{ value.cbegin(), value.cend() };
				}
				case lexeme_type::literal_string:
				{
					return '"' + std::string{ value.cbegin(), value.cend() } +'"';
				}
				default:
				{
					return to_string(type);
				}
			}
		}
	};
}
