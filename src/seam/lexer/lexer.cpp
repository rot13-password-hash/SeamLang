#include "lexer.hpp"
#include "../utils/exception.hpp"

#include <sstream>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

namespace seam::lexer
{
	using lexeme_map_t = std::unordered_map<std::string_view, lexeme_type>;
	const lexeme_map_t keyword_map
	{
		{ "fn", lexeme_type::kw_fn },
		{ "as", lexeme_type::kw_as },
		{ "return", lexeme_type::kw_return },
		{ "type", lexeme_type::kw_type },
		{ "try", lexeme_type::kw_try },
		{ "catch", lexeme_type::kw_catch },
		{ "switch", lexeme_type::kw_switch },
		{ "throw", lexeme_type::kw_throw },
		{ "true", lexeme_type::kw_true },
		{ "false", lexeme_type::kw_false },
		{ "while", lexeme_type::kw_while },
		{ "for", lexeme_type::kw_for },
		{ "if", lexeme_type::kw_if },
		{ "elseif", lexeme_type::kw_elseif },
		{ "else", lexeme_type::kw_else },
	};

	const lexeme_map_t symbol_map
	{
		{ "+", lexeme_type::symbol_add },
		{ "+=", lexeme_type::symbol_add_assign },
		{ "-", lexeme_type::symbol_minus },
		{ "-=", lexeme_type::symbol_minus_assign },
		{ "*", lexeme_type::symbol_multiply },
		{ "*=", lexeme_type::symbol_multiply_assign },
		{ "%", lexeme_type::symbol_mod },
		{ "(", lexeme_type::symbol_open_parenthesis },
		{ ")", lexeme_type::symbol_close_parenthesis },
		{ "[", lexeme_type::symbol_open_bracket },
		{ "]", lexeme_type::symbol_close_bracket },
		{ "{", lexeme_type::symbol_open_brace },
		{ "}", lexeme_type::symbol_close_brace },
		{ "->", lexeme_type::symbol_arrow },
		{ "=", lexeme_type::symbol_equals },
		{ "!", lexeme_type::symbol_not },
		{ "?", lexeme_type::symbol_question_mark },
		{ ":", lexeme_type::symbol_colon },
		{ ":=", lexeme_type::symbol_colon_equals },
		{ ",", lexeme_type::symbol_comma },
		{ "==", lexeme_type::symbol_eq },
		{ "!=", lexeme_type::symbol_neq },
		{ "<", lexeme_type::symbol_lt },
		{ "<=", lexeme_type::symbol_lteq },
		{ ">", lexeme_type::symbol_gt },
		{ ">=", lexeme_type::symbol_gteq },
		{ "&&", lexeme_type::symbol_and },
		{ "||", lexeme_type::symbol_or },
	};

	const std::unordered_set<std::string> attributes = { "constructor", "export" };
	
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

	void lexer::skip_comment(lexeme& ref)
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

	void lexer::lex_string_literal(lexeme& ref)
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
				ref.value = source_.substr(start_offset, read_offset_ - start_offset);
				consume_character();
				return;
			}

			consume_character();
		}
	}

	void lexer::lex_number_literal(lexeme& ref)
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

		ref.value = source_.substr(start_offset, read_offset_ - start_offset);
		ref.type = lexeme_type::literal_number;
	}

	void lexer::lex_keyword_or_identifier(lexeme& ref)
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
				ref.type = lexeme_type::identifier;
				ref.value = source_.substr(start_offset, read_offset_ - start_offset);
				break;
			}

			consume_character();
		}

		if (can_be_keyword)
		{
			if (const auto keyword_type = keyword_map.find(source_.substr(start_offset, read_offset_ - start_offset)); keyword_type != keyword_map.cend())
			{
				ref.type = keyword_type->second;
			}
		}
	}

	void lexer::lex_attribute(lexeme& ref)
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
				ref.type = lexeme_type::attribute;

				const auto proposed_attribute = source_.substr(start_offset, read_offset_ - start_offset);
				if (attributes.find(std::string{ proposed_attribute }) == attributes.cend())
				{
					std::stringstream error_message;
					error_message << "unknown attribute: '" << proposed_attribute << "'";
					throw utils::lexical_exception{ current_position(), error_message.str() };
				}

				ref.value = proposed_attribute;
				break;
			}

			consume_character();
		}
	}

	void lexer::lex_symbol(lexeme& ref)
	{
		// TODO: REFACTOR THIS
		const auto start_offset = read_offset_;

		consume_character();
		lexeme_map_t::const_iterator symbol;
		if (peek_character(1) != eof_character && (symbol = symbol_map.find(source_.substr(start_offset, 2))) != symbol_map.cend())
		{
			consume_character();
			ref.type = symbol->second;
		}
		else if (symbol = symbol_map.find(source_.substr(start_offset, 1)); symbol != symbol_map.cend())
		{
			ref.type = symbol->second;
		}
		else
		{
			throw utils::lexical_exception{
				current_position(),
				"unexpected symbol"
			};
		}
	}
	
	lexer::lexer(std::shared_ptr<types::module> current_module, const std::string_view& source)
		: current_module(current_module), source_(source) {}



	void lexer::lex(lexeme& ref)
	{
		skip_whitespace();

		ref.position = current_position();

		switch (peek_character())
		{
			case eof_character: // End of file.
			{
				ref.type = lexeme_type::eof;
				break;
			}
			case '/': // Comment or division operations.
			{
				consume_character();
				const auto next_character = peek_character();

				if (next_character == '/') // Is a comment.
				{
					skip_comment(ref);
					next_lexeme();
					break;
				}

				if (next_character == '=') // Divide-Assign operator.
				{
					consume_character();
					ref.type = lexeme_type::symbol_divide_assign;
					break;
				}
				ref.type = lexeme_type::symbol_divide; // Otherwise - just divide symbol.
				break;
			}
			case '"': // String literal.
			{
				ref.type = lexeme_type::literal_string;
				lex_string_literal(ref);
				break;
			}
			case '@': // Attribute.
			{
				lex_attribute(ref);
				break;
			}			
			default:
			{
				if (is_start_identifier_char(peek_character())) // Must be keyword or identifier.
				{
					lex_keyword_or_identifier(ref);
					return;
				}
					
				if (std::isdigit(peek_character())) // Number literal.
				{
					lex_number_literal(ref);
					return;
				}
					
				lex_symbol(ref);
				break;
			}
		}	
	}

	lexeme& lexer::peek_lexeme()
	{
		if (!peeked_lexeme_)
		{
			peeked_lexeme_ = lexeme{};
			lex(*peeked_lexeme_);
		}
		return *peeked_lexeme_;
	}

	void lexer::next_lexeme()
	{
		if (peeked_lexeme_)
		{
			current_ = peeked_lexeme_;
			peeked_lexeme_.reset();
		}
		else
		{
			lex(*current_);
		}
	}
}
