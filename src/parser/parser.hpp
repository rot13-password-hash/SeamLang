#pragma once

#include <llvm/Support/Error.h>

#include "../ir/ast/expression.hpp"
#include "../lexer/lexer.hpp"

namespace seam::parser
{
	class parser
	{
		std::string_view filename_; // name of file currently being parsed.
		lexer::lexer lexer_; // current lexer instance.

		/**
		 * Checks whether current lexeme type matches expected.
		 *
		 * @param type expected lexeme type.
		 * @param consume whether lexeme should be consumed automatically.
		 *
		 * @note For a better understanding of this method, it is encouraged that you
		 *		 read the documentation relating to the llvm::Error class.
		 *
		 * @returns llvm::Error if current lexeme does not match expected, otherwise success.
		 */
		llvm::Error expect(lexer::lexeme_type type, bool consume = false);

		/**
		 * Parses a type.
		 *
		 * Attempts to parse a type, and returns internal representation
		 * of a type.
		 *
		 * @returns a type.
		 */
		llvm::Expected<ir::ast::type> parse_type();

		/**
		 * Parses a parameter.
		 *
		 * Attempts to parse a parameter, and returns internal representation
		 * of a parameter.
		 *
		 * @returns a parameter.
		 */
		llvm::Expected<ir::ast::parameter> parse_parameter();

		/**
		 * Parses a parameter list.
		 *
		 * Attempts to parse a parameter list, and returns internal representation
		 * of a parameter list (ast::parameter_list, vector of parameters).
		 *
		 * @returns a vector of parameters.
		 */
		llvm::Expected<ir::ast::parameter_list> parse_parameter_list();

		llvm::Expected<ir::ast::expression_list> parse_expression_list();
		
		llvm::Expected<std::unique_ptr<ir::ast::call_expression>> parse_call_expression(std::unique_ptr<ir::ast::expression> function);
		
		/**
		 * Parses any generic prefix expression.
		 *
		 * Attempts to parse a prefix expression and returns internal representation of
		 * said expression.
		 *
		 * @returns a unique pointer to an expression node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::expression>> parse_prefix_expression();
		
		/**
		 * Parses any generic expression.
		 *
		 * Attempts to parse an expression and returns internal representation of
		 * said expression.
		 *
		 * @returns a unique pointer to an expression node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::expression>> parse_expression();
		
		/**
		 * Parses a block statement.
		 *
		 * A block is considered to be the main body of any method,
		 * and can contain both statements and expressions.
		 *
		 * @returns a unique pointer to a block ast node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::block>> parse_block_statement();

		/**
		 * Parses a function definition statement.
		 *
		 * @returns a unique pointer to a function definition ast node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::function_declaration>> parse_function_definition_statement();
		
		/**
		 * Parses a type definition statement.
		 *
		 * @returns a unique pointer to a type definition ast node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::type_definition>> parse_type_definition_statement();
		
		/**
		 * Parses a restricted statement.
		 *
		 * A restricted statement is any of the following:
		 *
		 * - function
		 * - type
		 * - export???
		 *
		 * @returns a unique pointer to a restricted statement ast node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::restricted_statement>> parse_restricted_statement();
		
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
		 * @returns a unique pointer to a restricted block ast node when successful, otherwise llvm::Error.
		 */
		llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parse_restricted_block_statement(bool is_type_scope = false);
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
