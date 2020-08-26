#include "lexer/lexer.h"

int main()
{
	seam::lexer::lexer lexer(R"(
		fn main() @constructor
		{
		  test()
		}
	)");

	lexer.next_lexeme();
	auto next_lexeme = lexer.current_lexeme();
	while (next_lexeme.type != seam::lexer::lexeme_type::eof)
	{
		lexer.next_lexeme();
		next_lexeme = lexer.current_lexeme();
	}
}