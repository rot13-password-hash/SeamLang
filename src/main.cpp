#include <llvm/Support/WithColor.h>

#include "parser/parser.h"
#include "utils/exception.h"

int main()
{
	seam::parser::parser parser("test", R"(
		fn test()
		{

		}

		fn main() @constructor
		{
			test() 
		}
	)");

	llvm::ExitOnError exitOnErr{};
	try
	{
		auto root = parser.parse();
		exitOnErr(root.takeError());
	}
	catch (const seam::utils::exception& ex)
	{
		llvm::errs() << ex.position.line << ':' << ex.position.column << ": ";
		llvm::WithColor::error() << ex.what() << '\n';
	}
	catch (const std::exception& ex)
	{
		llvm::WithColor::error();
		llvm::errs() << ex.what() << '\n';
	}
}
