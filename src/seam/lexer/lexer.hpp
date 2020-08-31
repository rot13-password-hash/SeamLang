#pragma once

#include "lexeme.hpp"
#include "../utils/position.hpp"
#include "../types/module.hpp"

#include <memory>
#include <optional>

namespace seam::lexer
{
	/**
	 * Implementation of lexer.
	 */
	class lexer
	{
		constexpr static char eof_character = -1;

		std::shared_ptr<types::module> current_module;

		std::string_view source_;

		std::size_t read_offset_ = 0;
		std::size_t line_ = 1;
		std::size_t line_start_offset_ = 0;
		
		std::optional<lexeme> current_;
		std::optional<lexeme> peeked_lexeme_;

		[[nodiscard]] utils::position current_position() const;
		[[nodiscard]] char peek_character(std::size_t offset = 0) const;
		void consume_character();
		
		void skip_whitespace();
		void skip_comment(lexeme& ref);
		void lex_string_literal(lexeme& ref);
		void lex_number_literal(lexeme& ref);
		void lex_keyword_or_identifier(lexeme& ref);
		void lex_attribute(lexeme& ref);
		void lex_symbol(lexeme& ref);

		void lex(lexeme& ref);
	public:
		/**
		 * Initialise lexer with source to lex.
		 *
		 * @param source source to lex.
		 */
		explicit lexer(std::shared_ptr<types::module> current_module, const std::string_view& source);

		/**
		 * Peeks a future lexeme.
		 *
		 * @note does not automatically move to the next lexeme.
		 * @return current lexeme held in lexer.
		 */
		lexeme& peek_lexeme();
		
		/**
		 * Retrieves the current lexeme.
		 *
		 * @note does not automatically move to the next lexeme.
		 * @return current lexeme held in lexer.
		 */
		[[nodiscard]] const lexeme& current_lexeme() const { return *current_; }

		/**
		 * Moves the lexer to the next lexeme.
		 *
		 * @throws lexical_exception if lexing fails.
		 */
		void next_lexeme();
	};
}
