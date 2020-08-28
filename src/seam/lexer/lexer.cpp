#include "lexer.hpp"
#include "../utils/exception.hpp"

#include <cctype>
#include <unordered_map>

namespace seam::lexer
{
	using lexeme_map_t = std::unordered_map<std::string_view, lexeme_type>;
	const lexeme_map_t keyword_map
	{
		{ "fn", lexeme_type::kw_fn },
		{ "extern", lexeme_type::kw_extern },
		{ "as", lexeme_type::kw_as },
		{ "return", lexeme_type::kw_return },
		{ "type", lexeme_type::kw_type },
		{ "try", lexeme_type::kw_try },
		{ "catch", lexeme_type::kw_catch },
		{ "switch", lexeme_type::kw_switch },
		{ "throw", lexeme_type::kw_throw },
		{ "true", lexeme_type::kw_true },
		{ "false", lexeme_type::kw_false },
	};

	const lexeme_map_t symbol_map
	{
		{ "+", lexeme_type::symbol_add },
		{ "+=", lexeme_type::symbol_add_assign },
		{ "-", lexeme_type::symbol_minus },
		{ "-=", lexeme_type::symbol_minus_assign },
		{ "*", lexeme_type::symbol_multiply },
		{ "*=", lexeme_type::symbol_multiply_assign },
		{ "(", lexeme_type::symbol_open_parenthesis },
		{ ")", lexeme_type::symbol_close_parenthesis },
		{ "[", lexeme_type::symbol_open_bracket },
		{ "]", lexeme_type::symbol_close_bracket },
		{ "{", lexeme_type::symbol_open_brace },
		{ "}", lexeme_type::symbol_close_brace },
		{ "->", lexeme_type::symbol_arrow },
		{ "=", lexeme_type::symbol_equals },
		{ ":=", lexeme_type::symbol_declare },
		{ "?", lexeme_type::symbol_question_mark },
		{ ":", lexeme_type::symbol_colon },
		{ ",", lexeme_type::symbol_comma },
	};
	
	bool is_start_identifier_char(const char value)
	{
		return std::isalpha(value) || value == '_';
	}

	bool is_identifier_char(const char value)
	{
		return std::isalnum(value) || value == '_';
	}

	bool is_keyword_char(const char value)
	{
		return value != '_' && !std::isupper(value);
	}
	
	utils::position lexer::current_position() const
	{
		return { line_, read_offset_ - line_start_offset_ };
	}
	
	char lexer::peek_character(const std::size_t offset) const
	{
		return read_offset_ + offset >= source_.length() ? eof_character: source_[read_offset_ + offset];
	}
	
	void lexer::consume_character()
	{
		if (peek_character() == '\n')
		{
			++line_;
			line_start_offset_ = read_offset_;
		}
		++read_offset_;
	}

	void lexer::skip_whitespace()
	{
		char next_character;
		while ((next_character = peek_character()) != eof_character && std::isspace(next_character))
		{
			consume_character();
		}
	}

	void lexer::skip_comment()
	{
		consume_character();
		const auto is_long_comment = peek_character() == '/';

		while (true)
		{
			const auto next_character = peek_character();
			if (!is_long_comment && next_character == '\n')
			{
				consume_character();
				return;
			}

			// There has got to be a better way to do this...
			if (is_long_comment && next_character == '/' && peek_character(1) == '/' && peek_character(2) == '/')
			{
				consume_character();
				consume_character();
				consume_character();
				return;
			}

			if (is_long_comment && next_character == eof_character)
			{
				throw utils::lexical_exception { current_position(), "unterminated long comment" };
			}

			consume_character();
		}
	}

	void lexer::lex_string_literal()
	{
		consume_character();
		const auto start_offset = read_offset_;
		
		while (true)
		{
			if (peek_character() == '\\' && peek_character(1) == '"') // String escape
			{
				consume_character();
				consume_character();
				continue;
			}

			if (peek_character() == '"')
			{
				current_.value = source_.substr(start_offset, read_offset_ - start_offset);
				consume_character();
				return;
			}

			consume_character();
		}
	}

	void lexer::lex_number_literal()
	{
		const auto is_hex = peek_character() == '0' && peek_character(1) == 'x';
		auto is_float = false;

		const auto start_offset = read_offset_;

		if (is_hex)
		{
			consume_character();
			consume_character(); // Consume 0x.
		}

		while (true)
		{
			if (peek_character() == '.')
			{
				is_float = true;
				consume_character();
			}

			if (is_hex && is_float)
			{
				throw utils::lexical_exception{
					current_position(),
					"malformed number"
				};
			}

			const auto next_char = peek_character(); // Not yet parsed correctly I think?
			if ((is_hex ? std::isxdigit(next_char) : std::isdigit(next_char)) || next_char == '_')
			{
				consume_character();
				//peek_character();
			}
			else
			{
				break;
			}
		}

		current_.value = source_.substr(start_offset, read_offset_ - start_offset);
		current_.type = lexeme_type::literal_number;
	}

	void lexer::lex_keyword_or_identifier()
	{
		auto can_be_keyword = is_keyword_char(peek_character());

		const auto start_offset = read_offset_;
		consume_character();

		while (true)
		{
			const auto next_char = peek_character();

			if (!is_keyword_char(next_char))
			{
				can_be_keyword = false;
			}

			if (!is_identifier_char(next_char))
			{
				current_.type = lexeme_type::identifier;
				current_.value = source_.substr(start_offset, read_offset_ - start_offset);
				break;
			}

			consume_character();
		}

		if (can_be_keyword)
		{
			if (const auto keyword_type = keyword_map.find(source_.substr(start_offset, read_offset_ - start_offset)); keyword_type != keyword_map.cend())
			{
				current_.type = keyword_type->second;
			}
		}
	}

	void lexer::lex_attribute()
	{
		consume_character();

		const auto start_offset = read_offset_;

		if (!is_start_identifier_char(peek_character()))
		{
			throw utils::lexical_exception{
				current_position(),
				"unexpected symbol"
			};
		}

		consume_character();

		while (true)
		{
			const auto next_char = peek_character();

			if (!is_identifier_char(next_char))
			{
				current_.type = lexeme_type::attribute;
				current_.value = source_.substr(start_offset, read_offset_ - start_offset);
				break;
			}

			consume_character();
		}
	}

	void lexer::lex_symbol()
	{
		// TODO: REFACTOR THIS
		const auto start_offset = read_offset_;

		consume_character();
		lexeme_map_t::const_iterator symbol;
		if (peek_character(1) != eof_character && (symbol = symbol_map.find(source_.substr(start_offset, 2))) != symbol_map.cend())
		{
			consume_character();
			current_.type = symbol->second;
		}
		else if (symbol = symbol_map.find(source_.substr(start_offset, 1)); symbol != symbol_map.cend())
		{
			current_.type = symbol->second;
		}
		else
		{
			throw utils::lexical_exception{
				current_position(),
				"unexpected symbol"
			};
		}
	}
	
	lexer::lexer(const std::string_view& source)
		: source_(source) {}

	void lexer::next_lexeme()
	{
		skip_whitespace();

		current_.position = current_position();

		switch (peek_character())
		{
			case eof_character: // End of file.
			{
				current_.type = lexeme_type::eof;
				break;
			}
			case '/': // Comment or division operations.
			{
				consume_character();
				const auto next_character = peek_character();

				if (next_character == '/') // Is a comment.
				{
					skip_comment();
					next_lexeme();
					break;
				}

				if (next_character == '=') // Divide-Assign operator.
				{
					consume_character();
					current_.type = lexeme_type::symbol_divide_assign;
					break;
				}
				current_.type = lexeme_type::symbol_divide; // Otherwise - just divide symbol.
				break;
			}
			case '"': // String literal.
			{
				current_.type = lexeme_type::literal_string;
				lex_string_literal();
				break;
			}
			case '@': // Attribute.
			{
				lex_attribute();
				break;
			}			
			default:
			{
				if (is_start_identifier_char(peek_character())) // Must be keyword or identifier.
				{
					lex_keyword_or_identifier();
					return;
				}

				if (std::isdigit(peek_character())) // Number literal.
				{
					lex_number_literal();
					return;
				}

				lex_symbol();
				break;
			}
		}
	}
}
