#include <sstream>

#include "parser.h"
#include "../utils/error.h"

namespace seam::parser
{
	llvm::Error parser::expect(lexer::lexeme_type type, bool consume)
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

	llvm::Expected<std::unique_ptr<ir::ast::restricted_statement>> parser::parse_restricted_statement() const
	{
		const auto& current_lexeme = lexer_.current_lexeme();
		
		switch (current_lexeme.type)
		{
			case lexer::lexeme_type::kw_fn:
			{
				// TODO: Parse Function Definition
				break;
			}
			case lexer::lexeme_type::kw_type:
			{
				// TODO: Parse Type Definition
				break;
			}
			case lexer::lexeme_type::kw_extern:
			{
				// TODO: Parse Extern Definitions, and actually consider whether this should be here?
				break;
			}
			default:
			{
				std::stringstream error_message;
				error_message << "unexpected identifier " << current_lexeme.to_string() << " in restricted namespace, expected a type or function definition";
				return llvm::make_error<utils::error_info>(filename_, lexer_.current_lexeme().position, error_message.str());
			}
		}
		
		return nullptr;
	}

	llvm::Expected<std::unique_ptr<ir::ast::restricted_block>> parser::parse_restricted_block_statement(const bool is_type_scope) const
	{
		const auto start_position = lexer_.current_lexeme().position;

		std::vector<std::unique_ptr<ir::ast::restricted_statement>> body;
		while (true)
		{
			const auto& current_lexeme = lexer_.current_lexeme();

			if (!is_type_scope && current_lexeme.type == lexer::lexeme_type::eof
				|| is_type_scope && current_lexeme.type == lexer::lexeme_type::symbol_close_brace)
			{
				break;
			}

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
		lexer_.next_lexeme();

		auto root = parse_restricted_block_statement();
		if (!root)
		{
			return root.takeError();
		}
		
		// Check whether last lexeme is EOF
		if (auto error = expect(lexer::lexeme_type::eof))
		{
			return std::move(error);
		}

		// TODO: Invoke parser passes here!

		return root;
	}
}
