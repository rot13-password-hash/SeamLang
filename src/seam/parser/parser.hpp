#pragma once

#include "../ir/ast/expression.hpp"
#include "../lexer/lexer.hpp"
#include "../types/module.hpp"

#include <memory>

namespace seam::parser
{
	class parser
	{
		std::shared_ptr<types::module> current_module;

		std::string_view filename_; // name of file currently being parsed.
		lexer::lexer lexer_; // current lexer instance.

		/**
		 * 
		 */
		void skip_statement();
		
		/**
		 * Checks whether current lexeme type matches expected.
		 *
		 * @param type expected lexeme type.
		 * @param consume whether lexeme should be consumed automatically.

		 */
		void expect(lexer::lexeme_type type, bool consume = false);

		/**
		 * Parses a type.
		 *
		 * Attempts to parse a type, and returns internal representation
		 * of a type.
		 *
		 * @returns a type.
		 */
		std::unique_ptr<ir::ast::unresolved_type> parse_type();

		/**
		 * Parses a parameter.
		 *
		 * Attempts to parse a parameter, and returns internal representation
		 * of a parameter.
		 *
		 * @returns a parameter.
		 */
		ir::ast::parameter parse_parameter();

		/**
		 * Parses a parameter list.
		 *
		 * Attempts to parse a parameter list, and returns internal representation
		 * of a parameter list (ast::parameter_list, vector of parameters).
		 *
		 * @returns a vector of parameters.
		 */
		ir::ast::parameter_list parse_parameter_list();

		ir::ast::expression::expression_list parse_expression_list();
		
		std::unique_ptr<ir::ast::expression::call> parse_call_expression(std::unique_ptr<ir::ast::expression::expression> function);
		
		/**
		 * Parses any generic prefix expression.
		 *
		 * Attempts to parse a prefix expression and returns internal representation of
		 * said expression.
		 *
		 * @returns a unique pointer to an expression node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::expression::expression> parse_prefix_expression();
		
		/**
		 * Parses any generic expression.
		 *
		 * Attempts to parse an expression and returns internal representation of
		 * said expression.
		 *
		 * @returns a unique pointer to an expression node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::expression::expression> parse_expression();

		std::unique_ptr<ir::ast::statement::ret> parse_return_statement();
		
		/**
		 * Parses a block statement.
		 *
		 * A block is considered to be the main body of any method,
		 * and can contain both statements and expressions.
		 *
		 * @returns a unique pointer to a block ast node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::statement::block> parse_block_statement();

		/**
		 * Parses a function definition statement.
		 *
		 * @returns a unique pointer to a function definition ast node when successful, otherwise throws an exception.
		 */
		std::shared_ptr<types::function_signature> parse_function_signature();

		/**
		 * Parses a function definition statement.
		 *
		 * @returns a unique pointer to a function definition ast node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::statement::function_definition> parse_function_definition_statement();
		
		/**
		 * Parses a type definition statement.
		 *
		 * @returns a unique pointer to a type definition ast node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::statement::type_definition> parse_type_definition_statement();
		
		/**
		 * Parses a restricted statement.
		 *
		 * A restricted statement is any of the following:
		 *
		 * - function
		 * - type
		 * - extern
		 *
		 * @returns a unique pointer to a restricted statement ast node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::statement::restricted> parse_restricted_statement();
		
		/**
		 * Parses a restricted block statement.
		 *
		 * A restricted block is considered to be a top-level block which can consist of
		 * any of the following:
		 *
		 * - functions
		 * - types
		 * - extern
		 *
		 * @param is_type_scope whether we're parsing in a type scope.
		 * @returns a unique pointer to a restricted block ast node when successful, otherwise throws an exception.
		 */
		std::unique_ptr<ir::ast::statement::restricted_block> parse_restricted_block_statement(bool is_type_scope = false);
	public:
		/**
		 * Initialise parser with name of file being parsed, as well
		 * as its respective source.
		 *
		 * @param current_module the module to be parsed.
		 * @param filename name of file to be parsed.
		 * @param source source of file to parse.
		 */
		explicit parser(std::shared_ptr<types::module> current_module, const std::string_view filename, const std::string_view source);

		/**
		 * TODO: Comment this
		 */
		std::unique_ptr<ir::ast::statement::restricted_block> parse();
	};
}
