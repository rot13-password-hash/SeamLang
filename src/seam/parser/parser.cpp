#include <sstream>

#include "parser.hpp"
#include "../utils/exception.hpp"
#include "passes/pass.hpp"

namespace seam::parser
{
	void parser::expect(const lexer::lexeme_type type, const bool consume)
	{
		const auto& current_lexeme = lexer_.current_lexeme();

		if (current_lexeme.type != type)
		{
			std::stringstream error_message;
			error_message << "expected " << lexer::lexeme::to_string(type) << ", got " << lexer_.current_lexeme().to_string();
			throw utils::parser_exception{ current_lexeme.position, error_message.str() };
		}

		if (consume)
		{
			lexer_.next_lexeme();
		}
	}

	std::unique_ptr<ir::ast::unresolved_type> parser::parse_type()
	{
		const auto start_position = lexer_.current_lexeme().position;
		expect(lexer::lexeme_type::identifier);

		auto target_type_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		if (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_question_mark)
		{
			lexer_.next_lexeme();

			return std::make_unique<ir::ast::unresolved_type>(
				utils::position_range{ start_position, lexer_.current_lexeme().position },
				std::move(target_type_name),
				true
			);
		}

		return std::make_unique<ir::ast::unresolved_type>(
			utils::position_range{ start_position, lexer_.current_lexeme().position },
			std::move(target_type_name),
			false
		);
	}

	ir::ast::parameter parser::parse_parameter()
	{
		// Verify next token is parameter name (identifier)
		expect(lexer::lexeme_type::identifier);

		const auto parameter_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		// Check for colon preceding parameter type
		expect(lexer::lexeme_type::symbol_colon, true);

		return ir::ast::parameter{ parse_type(), parameter_name };
	}

	ir::ast::parameter_list parser::parse_parameter_list()
	{
		ir::ast::parameter_list param_list;

		// Consume open parenthesis - (
		lexer_.next_lexeme();

		if (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_parenthesis)
		{
			param_list.emplace_back(parse_parameter());

			while (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_comma)
			{
				lexer_.next_lexeme();
				param_list.emplace_back(parse_parameter() );
			}
		}

		return param_list;
	}

	ir::ast::expression::expression_list parser::parse_expression_list()
	{
		ir::ast::expression::expression_list expression_list;

		if (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_parenthesis)
		{
			expression_list.emplace_back(parse_expression());

			while (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_comma)
			{
				lexer_.next_lexeme();
				expression_list.emplace_back(parse_expression());
			}
		}

		return expression_list;
	}

	std::unique_ptr<ir::ast::expression::call> parser::parse_call_expression(std::unique_ptr<ir::ast::expression::expression> function)
	{
		const auto start_position = lexer_.current_lexeme().position;

		lexer_.next_lexeme(); // Skip (

		// TODO: error recovery
		auto arguments = parse_expression_list();

		expect(lexer::lexeme_type::symbol_close_parenthesis, true);

		return std::make_unique<ir::ast::expression::call>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(function), std::move(arguments));
	}

	std::unique_ptr<ir::ast::expression::expression> parser::parse_prefix_expression()
	{
		const auto current_lexeme = lexer_.current_lexeme();
		const auto start_position = current_lexeme.position;

		switch (current_lexeme.type)
		{
		case lexer::lexeme_type::symbol_open_parenthesis: // (expr)
		{
			lexer_.next_lexeme();

			auto inner_expression = parse_expression();

			expect(lexer::lexeme_type::symbol_close_parenthesis, true);
			return inner_expression;
		}
		case lexer::lexeme_type::identifier: // variable
		{
			auto identifier_name = std::string{ current_lexeme.value };
			lexer_.next_lexeme();

			auto unresolved_var = std::make_unique<ir::ast::expression::unresolved_symbol>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(identifier_name));
			return std::make_unique<ir::ast::expression::variable>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(unresolved_var));
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '(' or identifier, got " << lexer_.current_lexeme().to_string();
			throw utils::parser_exception{ start_position, error_message.str() };
		}
		}
	}

	std::unique_ptr<ir::ast::expression::expression> parser::parse_expression()
	{
		const auto start_position = lexer_.current_lexeme().position;

		const auto current_lexeme = lexer_.current_lexeme();
		switch (const auto lexeme_type = current_lexeme.type)
		{
		case lexer::lexeme_type::kw_true:
		case lexer::lexeme_type::kw_false:
		{
			lexer_.next_lexeme(); // Consume lexeme.
			return std::make_unique<ir::ast::expression::bool_literal>(utils::position_range{ start_position, current_lexeme.position }, lexeme_type != lexer::lexeme_type::kw_false);
		}
		case lexer::lexeme_type::literal_number:
		{
			lexer_.next_lexeme();
			return std::make_unique<ir::ast::expression::string_literal>(utils::position_range{ start_position, current_lexeme.position }, std::string{ current_lexeme.value });
		}
		case lexer::lexeme_type::literal_string:
		{
			lexer_.next_lexeme();
			return std::make_unique<ir::ast::expression::string_literal>(utils::position_range{ start_position, current_lexeme.position }, std::string{ current_lexeme.value });
		}
		case lexer::lexeme_type::symbol_open_parenthesis: // (expr)
		case lexer::lexeme_type::identifier: // variable, function_name(<args>)
		{
			auto prefix_expression = parse_prefix_expression();

			auto expression = std::move(prefix_expression);
			while (lexer_.current_lexeme().position.line == start_position.line)
			{
				switch (lexer_.current_lexeme().type)
				{
				case lexer::lexeme_type::symbol_open_parenthesis:
				{
					expression = parse_call_expression(std::move(expression));
					continue;
				}
				default:
				{
					std::stringstream error_message;
					error_message << "expected function call or index, got " << current_lexeme.to_string();
					throw utils::parser_exception{ lexer_.current_lexeme().position, error_message.str() };
				}
				}
			}
			return expression;
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected expression, got " << current_lexeme.to_string();
			throw utils::parser_exception{ start_position, error_message.str() };
		}
		}
	}

	std::unique_ptr<ir::ast::statement::ret> parser::parse_return_statement()
	{
		const auto start_position = lexer_.current_lexeme().position;
		lexer_.next_lexeme();

		std::unique_ptr<ir::ast::expression::expression> expression = nullptr;
		if (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_brace
			&& lexer_.current_lexeme().position.line == start_position.line)
		{
			expression = parse_expression();
		}

		return std::make_unique<ir::ast::statement::ret>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(expression));
	}

	std::unique_ptr<ir::ast::statement::block> parser::parse_block_statement()
	{
		expect(lexer::lexeme_type::symbol_open_brace, true);

		const auto start_position = lexer_.current_lexeme().position;

		ir::ast::statement::statement_list body;
		while (true)
		{
			const auto& current_lexeme = lexer_.current_lexeme();
			const auto statement_start_position = current_lexeme.position;

			if (current_lexeme.type == lexer::lexeme_type::symbol_close_brace)
			{
				break;
			}

			// TODO: Finish block body parsing
			switch (current_lexeme.type)
			{
			case lexer::lexeme_type::kw_return:
			{
				body.emplace_back(parse_return_statement());
				break;
			}
			default:
			{
				auto expression = parse_expression();
				// TODO: Finish all this.
				break;
			}
			}
		}

		expect(lexer::lexeme_type::symbol_close_brace, true);

		return std::make_unique<ir::ast::statement::block>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(body));
	}

	std::shared_ptr<types::function_signature> parser::parse_function_signature()
	{
		const auto start_position = lexer_.current_lexeme().position;

		// Consume kw_fn
		lexer_.next_lexeme();

		// Verify next token is function name (identifier)
		expect(lexer::lexeme_type::identifier);

		// Store function name
		auto function_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		// Verify next token is open parenthesis (start of parameter_list)
		expect(lexer::lexeme_type::symbol_open_parenthesis);

		// Generate parameter list
		const auto param_list_pos = lexer_.current_lexeme().position;
		auto param_list = parse_parameter_list();

		expect(lexer::lexeme_type::symbol_close_parenthesis, true);

		// Check for explicit return type
		std::unique_ptr<ir::ast::unresolved_type> return_type;
		if (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_arrow)
		{
			lexer_.next_lexeme();
			return_type = parse_type();
		}
		else
		{
			// We do not allow for any implicit returns which
			// are not void, so we can simply set the return
			// type to void.
			return_type = std::make_unique<ir::ast::unresolved_type>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				"void",
				false);
		}

		// Check for attributes
		ir::ast::attribute_list attribute_list;
		while (lexer_.current_lexeme().type == lexer::lexeme_type::attribute)
		{
			const auto attribute = std::string{ lexer_.current_lexeme().value };
			if (attribute == "constructor" && !param_list.empty())
			{
				std::stringstream error_message;
				error_message << "constructor function '" << function_name << "' cannot have parameters";
				throw utils::parser_exception{ param_list_pos, error_message.str() };
			}
			attribute_list.insert(attribute);
			lexer_.next_lexeme();
		}

		return std::make_shared<types::function_signature>(current_module->name, std::move(function_name), std::move(return_type),
			std::move(param_list), std::move(attribute_list));
	}

	std::unique_ptr<ir::ast::statement::function_definition> parser::parse_function_definition_statement()
	{
		const auto start_position = lexer_.current_lexeme().position;
		auto signature = parse_function_signature();

		// Parse function body
		auto block = parse_block_statement();

		return std::make_unique<ir::ast::statement::function_definition>(utils::position_range{ start_position, lexer_.current_lexeme().position },
			signature, std::move(block));
	}

	std::unique_ptr<ir::ast::statement::type_definition> parser::parse_type_definition_statement()
	{
		const auto start_position = lexer_.current_lexeme().position;

		// Consume kw_type
		lexer_.next_lexeme();

		expect(lexer::lexeme_type::identifier);

		const auto type_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		const auto current_token = lexer_.current_lexeme();
		switch (current_token.type)
		{
		case lexer::lexeme_type::symbol_equals:
		{
			// Consume equals symbol
			lexer_.next_lexeme();

			return std::make_unique<ir::ast::statement::alias_type_definition>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				type_name, parse_type());
		}
		case lexer::lexeme_type::symbol_open_brace:
		{
			// Consume open brace symbol
			lexer_.next_lexeme();

			ir::ast::parameter_list fields;
			ir::ast::statement::restricted_list body;
			while (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_brace) // }
			{
				const auto& current_lexeme = lexer_.current_lexeme();
				if (current_lexeme.type == lexer::lexeme_type::identifier) // <identifier> : <type>
				{
					// <identifier>
					const auto field_name = std::string{ current_lexeme.value };
					lexer_.next_lexeme();

					// :
					expect(lexer::lexeme_type::symbol_colon, true);

					// <type>
					auto type = parse_type();

					fields.emplace_back(std::move(type), field_name);
				}
				else // methods, types, ...
				{
					body.emplace_back(parse_restricted_statement());
				}
			}

			// }
			expect(lexer::lexeme_type::symbol_close_brace, true);

			auto body_stat = std::make_unique<ir::ast::statement::restricted_block>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(body));

			return std::make_unique<ir::ast::statement::class_type_definition>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				type_name, std::move(fields), std::move(body_stat));
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected '=' or '{', got " << lexer_.current_lexeme().to_string();
			throw utils::parser_exception{ lexer_.current_lexeme().position, error_message.str() };
		}
		}
	}

	std::unique_ptr<ir::ast::statement::restricted> parser::parse_restricted_statement()
	{
		const auto& current_lexeme = lexer_.current_lexeme();

		switch (current_lexeme.type)
		{
		case lexer::lexeme_type::kw_fn: // Function Definition
		{
			return parse_function_definition_statement();
		}
		case lexer::lexeme_type::kw_type: // Type Definition
		{
			return parse_type_definition_statement();
		}
		default:
		{
			std::stringstream error_message;
			error_message << "unexpected identifier " << current_lexeme.to_string() << " in restricted namespace, expected a type or function definition";
			throw utils::parser_exception{ lexer_.current_lexeme().position, error_message.str() };
		}
		}
	}

	std::unique_ptr<ir::ast::statement::restricted_block> parser::parse_restricted_block_statement(bool is_type_scope)
	{
		const auto start_position = lexer_.current_lexeme().position;

		ir::ast::statement::restricted_list body;
		while (true)
		{
			const auto& current_lexeme = lexer_.current_lexeme();

			// Check whether we have any more lexemes to parse...
			//
			// Two cases:
			//
			// 1) If we're parsing a generic restricted block (global scope) we
			//    want to check for EOF.
			//
			// 2) Otherwise if we're parsing a type restricted block (type, duh!)
			//    we want to check for a closing brace.
			if (!is_type_scope && current_lexeme.type == lexer::lexeme_type::eof
				|| is_type_scope && current_lexeme.type == lexer::lexeme_type::symbol_close_brace)
			{
				break;
			}

			// Parse a restricted statement
			body.emplace_back(parse_restricted_statement());
		}

		return std::make_unique<ir::ast::statement::restricted_block>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(body));
	}

	parser::parser(std::shared_ptr<types::module> current_module, const std::string_view filename, const std::string_view source) :
		current_module(current_module), filename_(filename), lexer_(current_module, source) {}

	std::unique_ptr<ir::ast::statement::restricted_block> parser::parse()
	{
		// Get first lexeme.
		lexer_.next_lexeme();

		// Parse root.
		auto root = parse_restricted_block_statement();

		expect(lexer::lexeme_type::eof);

		passes::pass::run_passes(root.get());
		// TODO: Invoke parser passes here!

		return root;
	}
}
