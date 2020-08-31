#include <sstream>

#include "parser.hpp"

#include <iostream>

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

	std::unique_ptr<ir::ast::type_wrapper> parser::parse_type()
	{
		const auto start_position = lexer_.current_lexeme().position;
		expect(lexer::lexeme_type::identifier);

		auto target_type_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		bool is_optional = false;
		if (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_question_mark)
		{
			lexer_.next_lexeme();
			is_optional = true;
		}

		return std::make_unique<ir::ast::type_wrapper>(utils::position_range{ start_position, lexer_.current_lexeme().position },
			std::make_shared<ir::ast::unresolved_type>(std::move(target_type_name), is_optional));
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
				param_list.emplace_back(parse_parameter());
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

			
				
			return std::make_unique<ir::ast::expression::symbol_wrapper>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				std::make_unique<ir::ast::expression::unresolved_symbol>(std::move(identifier_name)));
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
		return parse_sub_expression().first;
	}

	struct priority
	{
		std::size_t left;
		std::size_t right;

		// if right > left then right associative
		priority(std::size_t left, std::size_t right) :
			left(left), right(right) {}

		priority(std::size_t priority) :
			left(priority), right(priority) {}
	};

	std::unordered_map<lexer::lexeme_type, priority> binary_priority
	{
		{ lexer::lexeme_type::symbol_add, 6 }, { lexer::lexeme_type::symbol_minus, 6 },
		{ lexer::lexeme_type::symbol_multiply, 7 }, { lexer::lexeme_type::symbol_divide, 7 }, { lexer::lexeme_type::symbol_mod, 7 },
		// comparison operators
		{ lexer::lexeme_type::symbol_eq, 3 }, { lexer::lexeme_type::symbol_neq, 3 },
		{ lexer::lexeme_type::symbol_lt, 3 }, { lexer::lexeme_type::symbol_lteq, 3 },
		{ lexer::lexeme_type::symbol_gt, 3 }, { lexer::lexeme_type::symbol_gteq, 3 },
		// logical operators
		{ lexer::lexeme_type::symbol_and, 2 }, { lexer::lexeme_type::symbol_or, 1 },
	};

	// higher than any binary priority
	std::size_t unary_priority = 8;

	std::pair<std::unique_ptr<ir::ast::expression::expression>, std::optional<lexer::lexeme_type>> parser::parse_sub_expression(std::size_t limit)
	{
		const auto start_position = lexer_.current_lexeme().position;

		std::unique_ptr<ir::ast::expression::expression> expression;
		// TODO: use while loop instead of recursion
		if (is_unary_operator(lexer_.current_lexeme().type))
		{
			auto operator_type = lexer_.current_lexeme().type;
			lexer_.next_lexeme();

			expression = std::make_unique<ir::ast::expression::unary>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				parse_sub_expression(unary_priority).first,
				operator_type);
			
		}
		else
		{
			expression = parse_simple_expression();
		}

		lexer::lexeme_type operator_type = lexer_.current_lexeme().type;
		while (true)
		{
			auto it = binary_priority.find(operator_type);
			if (it == binary_priority.cend() || limit >= it->second.left)
			{
				break;
			}

			lexer_.next_lexeme();

			auto [next_expression, next_operator_type] = parse_sub_expression(it->second.right);

			expression = std::make_unique<ir::ast::expression::binary>(utils::position_range{ start_position, lexer_.current_lexeme().position },
				std::move(expression), std::move(next_expression), operator_type);

			if (!next_operator_type)
			{
				break;
			}

			operator_type = next_operator_type.value();
		}

		return { std::move(expression), operator_type };
	}

	std::unique_ptr<ir::ast::expression::expression> parser::parse_simple_expression()
	{
		const auto start_position = lexer_.current_lexeme().position;

		// TODO: lmao copy???
		const auto current_lexeme = lexer_.current_lexeme();

		std::unique_ptr<ir::ast::expression::expression> expr;
		switch (const auto lexeme_type = current_lexeme.type) // + -> binary_expression(lhs, rhs, op)
		{
		case lexer::lexeme_type::kw_true:
		case lexer::lexeme_type::kw_false:
		{
			lexer_.next_lexeme(); // Consume lexeme.
			expr = std::make_unique<ir::ast::expression::bool_literal>(utils::position_range{ start_position, current_lexeme.position }, lexeme_type != lexer::lexeme_type::kw_false);
			break;
		}
		case lexer::lexeme_type::literal_number:
		{
			lexer_.next_lexeme();
			// TODO: number_wrapper
			expr = std::make_unique<ir::ast::expression::number_wrapper>(utils::position_range{ start_position, current_lexeme.position },
				std::make_unique<ir::ast::expression::unresolved_number>(std::string{ current_lexeme.value }));
			break;
		}
		case lexer::lexeme_type::literal_string:
		{
			lexer_.next_lexeme();
			expr = std::make_unique<ir::ast::expression::string_literal>(utils::position_range{ start_position, current_lexeme.position }, std::string{ current_lexeme.value });
			break;
		}
		case lexer::lexeme_type::symbol_open_parenthesis: // (expr)
		case lexer::lexeme_type::identifier: // variable, function_name(<args>)
		{
			return parse_primary_expression();
		}
		default:
		{
			std::stringstream error_message;
			error_message << "expected expression, got " << current_lexeme.to_string();
			throw utils::parser_exception{ start_position, error_message.str() };
		}
		}

		// auto lhs = parse_single_expression()
		// if add
		// op = get_op()
		// expr = binary_expression(lhs, parse_expression(3), op, binding_power = 0)

		return expr;
	}

	std::unique_ptr<ir::ast::expression::expression> parser::parse_primary_expression()
	{
		const auto start_position = lexer_.current_lexeme().position;

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
			}
			break;
		}

		return expression;
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

	std::unique_ptr<ir::ast::statement::while_loop> parser::parse_while_statement()
	{
		const auto start = lexer_.current_lexeme().position;
		lexer_.next_lexeme(); // collect kw_while

		expect(lexer::lexeme_type::symbol_open_parenthesis, true);
		auto condition = parse_expression();
		expect(lexer::lexeme_type::symbol_close_parenthesis, true);

		auto body = parse_block_statement();

		return std::make_unique<ir::ast::statement::while_loop>(
			utils::position_range{ start, lexer_.current_lexeme().position },
			std::move(condition),
			std::move(body));
	}

	std::unique_ptr<ir::ast::statement::if_stat> parser::parse_if_statement()
	{
		const auto start = lexer_.current_lexeme().position;
		lexer_.next_lexeme(); // collect kw_if

		expect(lexer::lexeme_type::symbol_open_parenthesis, true);
		auto condition = parse_expression();
		expect(lexer::lexeme_type::symbol_close_parenthesis, true);

		auto main_body = parse_block_statement();
		
		std::unique_ptr<ir::ast::statement::block> else_block;
		while (lexer_.current_lexeme().type == lexer::lexeme_type::kw_else
			|| lexer_.current_lexeme().type == lexer::lexeme_type::kw_elseif)
		{
			switch (lexer_.current_lexeme().type)
			{
				case lexer::lexeme_type::kw_else:
				{
					lexer_.next_lexeme();
					if (!else_block)
					{
						else_block = parse_block_statement();
						break;
					}
					throw utils::parser_exception{
						lexer_.current_lexeme().position,
						"cannot have more than one else",
					};
				}
				case lexer::lexeme_type::kw_elseif:
				{
					throw utils::parser_exception{
						lexer_.current_lexeme().position,
						"TODO: Check whether an else block exists, and create an if inside of it.",
					};
					break;
				}
				default:
				{
					break;
				}
			}
			
		}

		return std::make_unique<ir::ast::statement::if_stat>(
			utils::position_range { start, lexer_.current_lexeme().position },
			std::move(condition),
			std::move(main_body),
			std::move(else_block)
			);
	}

	std::unique_ptr<ir::ast::statement::block> parser::parse_block_statement()
	{
		expect(lexer::lexeme_type::symbol_open_brace, true);
		const auto start_position = lexer_.current_lexeme().position;

		const auto old_block = current_block;
		auto new_block = std::make_unique<ir::ast::statement::block>(utils::position_range{ start_position, lexer_.current_lexeme().position });
		new_block->parent = old_block;
		current_block = new_block.get();

		ir::ast::statement::statement_list body;
		while (true)
		{
			const auto current_lexeme = lexer_.current_lexeme();
			const auto statement_start_position = current_lexeme.position;

			if (current_lexeme.type == lexer::lexeme_type::symbol_close_brace)
			{
				break;
			}

			// TODO: Finish block body parsing
			switch (current_lexeme.type)
			{
			case lexer::lexeme_type::kw_if:
			{
				body.emplace_back(parse_if_statement());
				break;
			}
			case lexer::lexeme_type::kw_while:
			{
				body.emplace_back(parse_while_statement());
				break;
			}
			case lexer::lexeme_type::kw_for:
			{
				break;
			}
			case lexer::lexeme_type::kw_return:
			{
				body.emplace_back(parse_return_statement());
				break;
			}
			default:
			{
				// TODO: use parse_primary_expression if we only want to allow call + index
				auto expression = parse_expression();

				break;
			}
			}
		}

		expect(lexer::lexeme_type::symbol_close_brace, true);

		current_block = old_block;

		new_block->body = std::move(body);
		return new_block;
	}

	std::shared_ptr<ir::ast::function_signature> parser::parse_function_signature()
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
		std::unique_ptr<ir::ast::type_wrapper> return_type;
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
			return_type = std::make_unique<ir::ast::type_wrapper>(
				utils::position_range{ start_position, lexer_.current_lexeme().position },
				std::make_shared<ir::ast::unresolved_type>("void", false));
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

		return std::make_shared<ir::ast::function_signature>(current_module->name, std::move(function_name), std::move(return_type),
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
				const auto current_lexeme = lexer_.current_lexeme();
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
		const auto current_lexeme = lexer_.current_lexeme();

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
			const auto current_lexeme = lexer_.current_lexeme();

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