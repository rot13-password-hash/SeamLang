#pragma once

#include <llvm/Support/Error.h>


#include "../ir/ast/statement.h"
#include "../lexer/lexer.h"

namespace seam::parser
{
	class parser
	{
		std::string_view filename_;
		lexer::lexer lexer_;

		llvm::Error expect(lexer::lexeme_type type, bool consume = false);

		/**
		 * Parses a restricted statement.
		 *
		 * A restricted statement is any of the following:
		 *
		 * - function
		 * - type
		 * - export???
		 *
		 * @returns a unique pointer to a restricted statement.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::restricted_statement>> parse_restricted_statement() const;
		
		/**
		 * Parses a restricted block statement.
		 *
		 * A restricted block is considered to be a top-level block which can consist of
		 * any of the following:
		 *
		 * - functions
		 * - types
		 * - exports?
		 *
		 * @param is_type_scope whether we're parsing in a type scope.
		 * @returns a unique pointer to a restricted block.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parse_restricted_block_statement(bool is_type_scope = false) const;
	public:
		/**
		 * Initialise parser with name of file being parsed, as well
		 * as its respective source.
		 *
		 * @param filename name of file to be parsed.
		 * @param source source of file to parse.
		 */
		explicit parser(const std::string_view filename, const std::string_view source) :
			filename_(filename), lexer_(source) {}

		/**
		 * TODO: Comment this
		 */
		llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parse();
	};
}
