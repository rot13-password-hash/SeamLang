#include <llvm/Support/WithColor.h>

#include "seam/parser/parser.hpp"
#include "seam/utils/exception.hpp"

int main()
{
	seam::parser::parser parser("test", R"(
		type my_type = i32

		fn test()
		{

		}

		fn main() @constructor
		{
			test()
		}
	)");
	
	try
	{
		auto root = parser.parse();
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
