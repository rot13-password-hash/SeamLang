#pragma once

#include "lexeme.hpp"
#include "../utils/position.hpp"

namespace seam::lexer
{
	/**
	 * Implementation of lexer.
	 */
	class lexer
	{
		constexpr static char eof_character = -1;

		std::string_view source_;

		std::size_t read_offset_ = 0;
		std::size_t line_ = 1;
		std::size_t line_start_offset_ = 0;
		
		lexeme current_;

		[[nodiscard]] utils::position current_position() const;
		[[nodiscard]] char peek_character(std::size_t offset = 0) const;
		void consume_character();
		
		void skip_whitespace();
		void skip_comment();
		void lex_string_literal();
		void lex_number_literal();
		void lex_keyword_or_identifier();
		void lex_attribute();
		void lex_symbol();
	public:
		/**
		 * Initialise lexer with source to lex.
		 *
		 * @param source source to lex.
		 */
		explicit lexer(const std::string_view& source);

		/**
		 * Retrieves the current lexeme.
		 *
		 * @note does not automatically move to the next lexeme.
		 * @return current lexeme held in lexer.
		 */
		[[nodiscard]] const lexeme& current_lexeme() const { return current_; }

		/**
		 * Moves the lexer to the next lexeme.
		 *
		 * @throws lexical_exception if lexing fails.
		 */
		void next_lexeme();
	};
}
