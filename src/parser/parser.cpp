#include <sstream>

#include "parser.hpp"
#include "../utils/error.hpp"

namespace seam::parser
{
	llvm::Error parser::expect(const lexer::lexeme_type type, const bool consume)
	{
		const auto& current_lexeme = lexer_.current_lexeme();
		
		if (current_lexeme.type != type)
		{
			std::stringstream error_message;
			error_message << "expected " << lexer::lexeme::to_string(type) << ", got " << lexer_.current_lexeme().to_string();
			return llvm::make_error<utils::error_info>(filename_, current_lexeme.position, error_message.str());
		}

		if (consume)
		{
			lexer_.next_lexeme();
		}
		
		return llvm::Error::success();
	}

	llvm::Expected<ir::ast::type> parser::parse_type()
	{
		if (auto error = expect(lexer::lexeme_type::identifier))
		{
			return std::move(error);
		}

		auto target_type_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		if (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_question_mark)
		{
			lexer_.next_lexeme();

			return ir::ast::type { std::move(target_type_name), true };
		}

		return ir::ast::type { std::move(target_type_name), false };
	}

	llvm::Expected<ir::ast::parameter> parser::parse_parameter()
	{
		// Verify next token is parameter name (identifier)
		if (auto error = expect(lexer::lexeme_type::identifier))
		{
			return std::move(error);
		}

		const auto parameter_name = std::string { lexer_.current_lexeme().value };
		lexer_.next_lexeme();
		
		// Check for colon preceding parameter type
		if (auto error = expect(lexer::lexeme_type::symbol_colon, true))
		{
			return std::move(error);
		}

		auto type = parse_type();
		if (!type)
		{
			return type.takeError();
		}
		
		return ir::ast::parameter{ *type, parameter_name };
	}

	llvm::Expected<ir::ast::parameter_list> parser::parse_parameter_list()
	{
		ir::ast::parameter_list param_list;
		
		// Consume open parenthesis - (
		lexer_.next_lexeme();

		if (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_parenthesis)
		{
			auto first_param = parse_parameter();
			if (!first_param)
			{
				return first_param.takeError();
			}
			param_list.emplace_back(std::move(*first_param));

			while (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_comma)
			{
				lexer_.next_lexeme();

				auto param = parse_parameter();
				if (!param)
				{
					return param.takeError();
				}
				param_list.emplace_back(std::move(*param));
			}
		}
		
		return std::move(param_list);
	}

	llvm::Expected<ir::ast::expression_list> parser::parse_expression_list()
	{
		ir::ast::expression_list expression_list;

		if (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_parenthesis)
		{
			auto first_expression = parse_expression();
			if (!first_expression)
			{
				return first_expression.takeError();
			}
			expression_list.emplace_back(std::move(*first_expression));

			while (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_comma)
			{
				lexer_.next_lexeme();

				auto expression = parse_expression();
				if (!expression)
				{
					return expression.takeError();
				}
				expression_list.emplace_back(std::move(*expression));
			}
		}
		
		return std::move(expression_list);
	}

	llvm::Expected<std::unique_ptr<ir::ast::call_expression>> parser::parse_call_expression(std::unique_ptr<ir::ast::expression> function)
	{
		const auto start_position = lexer_.current_lexeme().position;

		lexer_.next_lexeme(); // Skip (

		// TODO: error recovery
		auto arguments = parse_expression_list();
		if (!arguments)
		{
			return arguments.takeError();
		}
		
		if (auto err = expect(lexer::lexeme_type::symbol_close_parenthesis, true))
		{
			return std::move(err);
		}

		return std::make_unique<ir::ast::call_expression>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(function), std::move(*arguments));
	}

	llvm::Expected<std::unique_ptr<ir::ast::expression>> parser::parse_prefix_expression()
	{
		const auto current_lexeme = lexer_.current_lexeme();
		const auto start_position = current_lexeme.position;

		switch (current_lexeme.type)
		{
			case lexer::lexeme_type::symbol_open_parenthesis: // (expr)
			{
				lexer_.next_lexeme();

				auto inner_expression = parse_expression();
				if (!inner_expression)
				{
					return inner_expression.takeError();
				}

				if (auto error = expect(lexer::lexeme_type::symbol_close_parenthesis, true))
				{
					return std::move(error);
				}
				return std::move(*inner_expression);
			}
			case lexer::lexeme_type::identifier: // variable
			{
				auto identifier_name = std::string{ current_lexeme.value };
				lexer_.next_lexeme();

				auto unresolved_var = std::make_unique<ir::ast::unresolved_variable>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(identifier_name));
				return std::make_unique<ir::ast::variable>(utils::position_range{ start_position, lexer_.current_lexeme().position }, std::move(unresolved_var));
			}
			default:
			{
				std::stringstream error_message;
				error_message << "expected '(' or identifier, got " << lexer_.current_lexeme().to_string();
				return llvm::make_error<utils::error_info>(filename_, start_position, error_message.str());
			}
		}
	}

	llvm::Expected<std::unique_ptr<ir::ast::expression>> parser::parse_expression()
	{
		const auto start_position = lexer_.current_lexeme().position;

		const auto current_lexeme = lexer_.current_lexeme();
		switch (const auto lexeme_type = current_lexeme.type)
		{
			case lexer::lexeme_type::kw_true:
			case lexer::lexeme_type::kw_false:
			{
				lexer_.next_lexeme(); // Consume lexeme.
				return std::make_unique<ir::ast::literal<bool>>(utils::position_range{ start_position, current_lexeme.position }, lexeme_type != lexer::lexeme_type::kw_false);
			}
			case lexer::lexeme_type::literal_number:
			{
				lexer_.next_lexeme();
				return std::make_unique<ir::ast::literal<std::string>>(utils::position_range{ start_position, current_lexeme.position }, std::string{ current_lexeme.value });
			}
			case lexer::lexeme_type::literal_string:
			{
				lexer_.next_lexeme();
				return std::make_unique<ir::ast::literal<std::string>>(utils::position_range{ start_position, current_lexeme.position }, std::string { current_lexeme.value });
			}
			case lexer::lexeme_type::symbol_open_parenthesis: // (expr)
			case lexer::lexeme_type::identifier: // variable, function_name(<args>)
			{
				auto prefix_expression = parse_prefix_expression();
				if (!prefix_expression)
				{
					return prefix_expression.takeError();
				}

				auto expression = std::move(*prefix_expression);
				while (lexer_.current_lexeme().position.line == start_position.line)
				{
					switch (lexer_.current_lexeme().type)
					{
						case lexer::lexeme_type::symbol_open_parenthesis:
						{
							auto call_args = parse_call_expression(std::move(expression));
							if (!call_args)
							{
								return call_args.takeError();
							}
							expression = std::move(*call_args);
							continue;
						}
						default:
						{
							std::stringstream error_message;
							error_message << "expected function call or index, got " << current_lexeme.to_string();
							return llvm::make_error<utils::error_info>(filename_, lexer_.current_lexeme().position, error_message.str());
						}
					}
				}
				return expression;
			}
			default:
			{
				std::stringstream error_message;
				error_message << "expected expression, got " << current_lexeme.to_string();
				return llvm::make_error<utils::error_info>(filename_, start_position, error_message.str());
			}
		}
	}

	llvm::Expected<std::unique_ptr<ir::ast::block>> parser::parse_block_statement()
	{
		if (auto error = expect(lexer::lexeme_type::symbol_open_brace, true))
		{
			return std::move(error);
		}

		const auto start_position = lexer_.current_lexeme().position;
		llvm::Error error = llvm::Error::success();		
		
		ir::ast::statement_list body;
		while (true)
		{
			const auto& current_lexeme = lexer_.current_lexeme();
			//const auto statement_start_position = current_lexeme.position;

			if (current_lexeme.type == lexer::lexeme_type::symbol_close_brace)
			{
				break;
			}

			// TODO: Finish block body parsing
			switch (current_lexeme.type)
			{
				case lexer::lexeme_type::kw_return:
				{
					// TODO: Parse return stat.
					break;
				}
				default:
				{
					break;
				}
			}
		}

		if (error)
		{
			return std::move(error);
		}
		
		if ((error = expect(lexer::lexeme_type::symbol_close_brace, true)))
		{
			return std::move(error);
		}
	
		return std::make_unique<ir::ast::block>(utils::position_range { start_position, lexer_.current_lexeme().position }, std::move(body));
	}

	llvm::Expected<std::unique_ptr<ir::ast::function_signature>> parser::parse_function_signature()
	{
		const auto start_position = lexer_.current_lexeme().position;

		// Consume kw_fn
		lexer_.next_lexeme();

		// Verify next token is function name (identifier)
		if (auto error = expect(lexer::lexeme_type::identifier))
		{
			return std::move(error);
		}

		// Store function name
		const auto function_name = lexer_.current_lexeme().value;		
		lexer_.next_lexeme();

		// Verify next token is open parenthesis (start of parameter_list)
		if (auto error = expect(lexer::lexeme_type::symbol_open_parenthesis))
		{
			return std::move(error);
		}

		// Generate parameter list
		auto param_list = parse_parameter_list();
		if (!param_list)
		{
			return param_list.takeError();
		}

		if (auto error = expect(lexer::lexeme_type::symbol_close_parenthesis, true))
		{
			return std::move(error);
		}

		// Check for explicit return type
		ir::ast::type return_type;
		if (lexer_.current_lexeme().type == lexer::lexeme_type::symbol_arrow)
		{
			auto parsed_return_type = parse_type();
			if (!parsed_return_type)
			{
				return parsed_return_type.takeError();
			}
			return_type = std::move(*parsed_return_type);
		}
		else
		{
			// We do not allow for any implicit returns which
			// are not void, so we can simply set the return
			// type to void.
			return_type.name = "void";
			return_type.is_optional = false;
		}

		// Check for attributes
		ir::ast::attribute_list attribute_list;
		while (lexer_.current_lexeme().type == lexer::lexeme_type::attribute)
		{
			attribute_list.insert(std::string { lexer_.current_lexeme().value });
			lexer_.next_lexeme();
		}
		// std::string{ function_name }, std::move(return_type),
		//	std::move(*param_list), std::move(attribute_list)
	}

	llvm::Expected<std::unique_ptr<ir::ast::function_definition>> parser::parse_function_definition_statement()
	{
		const auto start_position = lexer_.current_lexeme().position;
		auto signature = parse_function_signature();
		if (!signature)
		{
			return signature.takeError();
		}

		// Parse function body
		auto block = parse_block_statement();
		if (!block)
		{
			return block.takeError();
		}

		return std::make_unique<ir::ast::function_definition>(utils::position_range{ start_position, lexer_.current_lexeme().position },
			std::move(*signature), std::move(*block));
	}

	llvm::Expected<std::unique_ptr<ir::ast::extern_function_definition>> parser::parse_extern_function_definition_statement()
	{
		return nullptr;
	}

	llvm::Expected<std::unique_ptr<ir::ast::type_definition>> parser::parse_type_definition_statement()
	{
		const auto start_position = lexer_.current_lexeme().position;

		// Consume kw_type
		lexer_.next_lexeme();

		if (auto error = expect(lexer::lexeme_type::identifier))
		{
			return std::move(error);
		}
		
		const auto type_name = std::string{ lexer_.current_lexeme().value };
		lexer_.next_lexeme();

		const auto current_token = lexer_.current_lexeme();
		switch (current_token.type)
		{
			case lexer::lexeme_type::symbol_equals:
			{
				// Consume equals symbol
				lexer_.next_lexeme();

				auto target_type = parse_type();
				if (!target_type)
				{
					return target_type.takeError();
				}

				return std::make_unique<ir::ast::alias_type_definition>( utils::position_range { start_position, lexer_.current_lexeme().position },
					type_name, std::move(*target_type));
			}
			case lexer::lexeme_type::symbol_open_brace:
			{
				// Consume open brace symbol
				lexer_.next_lexeme();

				ir::ast::parameter_list fields;
				ir::ast::restricted_statement_list body;
				while (lexer_.current_lexeme().type != lexer::lexeme_type::symbol_close_brace) // }
				{
					const auto current_lexeme = lexer_.current_lexeme();
					if (current_lexeme.type == lexer::lexeme_type::identifier) // <identifier> : <type>
					{
						// <identifier>
						const auto field_name = std::string { current_lexeme.value };
						lexer_.next_lexeme();

						// :
						if (auto error = expect(lexer::lexeme_type::symbol_colon, true))
						{
							return std::move(error);
						}

						// <type>
						auto type = parse_type();
						if (!type)
						{
							return type.takeError();
						}

						fields.emplace_back(std::move(*type), field_name);
					}
					else // methods, types, ...
					{
						auto restricted_statement = parse_restricted_statement();
						if (!restricted_statement)
						{
							return restricted_statement.takeError();
						}

						body.emplace_back(std::move(*restricted_statement));
					}
				}

				// }
				if (auto error = expect(lexer::lexeme_type::symbol_close_brace, true))
				{
					return std::move(error);
				}
					
				auto body_stat = std::make_unique<ir::ast::restricted_block>(utils::position_range { start_position, lexer_.current_lexeme().position }, std::move(body));
					
				return std::make_unique<ir::ast::class_type_definition>(utils::position_range { start_position, lexer_.current_lexeme().position },
					type_name, std::move(fields), std::move(body_stat));
			}
			default:
			{
				std::stringstream error_message;
				error_message << "expected '=' or '{', got " << lexer_.current_lexeme().to_string();
				return llvm::make_error<utils::error_info>(filename_, lexer_.current_lexeme().position, error_message.str());
			}
		}
	}

	llvm::Expected<std::unique_ptr<ir::ast::restricted_statement>> parser::parse_restricted_statement()
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
			case lexer::lexeme_type::kw_extern: // Extern Definition
			{
				return parse_extern_function_definition_statement();
			}
			default:
			{
				std::stringstream error_message;
				error_message << "unexpected identifier " << current_lexeme.to_string() << " in restricted namespace, expected a type or function definition";
				return llvm::make_error<utils::error_info>(filename_, lexer_.current_lexeme().position, error_message.str());
			}
		}
	}

	llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parser::parse_restricted_block_statement(const bool is_type_scope)
	{
		const auto start_position = lexer_.current_lexeme().position;

		ir::ast::restricted_statement_list body;
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
			auto restricted_statement = parse_restricted_statement();
			if (!restricted_statement)
			{
				return restricted_statement.takeError();
			}

			body.push_back(std::move(*restricted_statement));
		}
		
		return std::make_unique<ir::ast::restricted_block>(utils::position_range { start_position, lexer_.current_lexeme().position }, std::move(body));
	}
	
	llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parser::parse()
	{
		// Get first lexeme.
		lexer_.next_lexeme();

		// Parse root.
		auto root = parse_restricted_block_statement();
		if (!root)
		{
			return root.takeError();
		}
		
		if (auto error = expect(lexer::lexeme_type::eof))
		{
			return std::move(error);
		}

		// TODO: Invoke parser passes here!
		
		return root;
	}
}
