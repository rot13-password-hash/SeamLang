#define CATCH_CONFIG_MAIN
#include "../3rdparty/catch2.h"
#include "../lexer/lexer.h"

TEST_CASE("Example lexed source", "[lexer]") {
	std::vector<seam::lexer::lexeme> expected_lexemes = {
		{ seam::lexer::lexeme_type::kw_fn },
		{ seam::lexer::lexeme_type::identifier, "test_arrow_method" },
		{ seam::lexer::lexeme_type::symbol_open_parenthesis },
		{ seam::lexer::lexeme_type::identifier, "arg" },
		{ seam::lexer::lexeme_type::symbol_colon },
		{ seam::lexer::lexeme_type::identifier, "i32" },
		{ seam::lexer::lexeme_type::symbol_close_parenthesis },
		{ seam::lexer::lexeme_type::symbol_arrow },
		{ seam::lexer::lexeme_type::identifier, "i32" },
		{ seam::lexer::lexeme_type::symbol_open_brace },
		{ seam::lexer::lexeme_type::kw_return },
		{ seam::lexer::lexeme_type::identifier, "arg" },
		{ seam::lexer::lexeme_type::symbol_add },
		{ seam::lexer::lexeme_type::literal_number, "1" },
		{ seam::lexer::lexeme_type::symbol_close_brace },
		{ seam::lexer::lexeme_type::kw_fn },
		{ seam::lexer::lexeme_type::identifier, "main" },
		{ seam::lexer::lexeme_type::symbol_open_parenthesis },
		{ seam::lexer::lexeme_type::symbol_close_parenthesis },
		{ seam::lexer::lexeme_type::attribute, "constructor" },
		{ seam::lexer::lexeme_type::symbol_open_brace },
		{ seam::lexer::lexeme_type::identifier, "test_arrow_method" },
		{ seam::lexer::lexeme_type::symbol_open_parenthesis },
		{ seam::lexer::lexeme_type::literal_number, "2" },
		{ seam::lexer::lexeme_type::symbol_close_parenthesis },
		{ seam::lexer::lexeme_type::symbol_close_brace },
		{ seam::lexer::lexeme_type::eof },
	};
	
    seam::lexer::lexer lexer(R"(
		fn test_arrow_method(arg: i32) -> i32
		{
			return arg + 1
		}

		fn main() @constructor
		{
			test_arrow_method(2)
		}
	)");	

	for (auto expected_lexeme: expected_lexemes)
	{
		lexer.next_lexeme();
		auto next_lexeme = lexer.current_lexeme();
		REQUIRE(next_lexeme.type == expected_lexeme.type);
		if (!expected_lexeme.value.empty())
		{
			REQUIRE(next_lexeme.value == expected_lexeme.value);
		}
	}
}