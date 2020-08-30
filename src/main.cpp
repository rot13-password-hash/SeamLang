#include <llvm/Support/WithColor.h>

#include "seam/parser/parser.hpp"
#include "seam/utils/exception.hpp"
#include "seam/types/module.hpp"
#include "seam/code_generation/code_generation.hpp"

#include <memory>

int main()
{
	const auto module = std::make_shared<seam::types::module>("test_module");

	seam::parser::parser parser(module, "test", R"(
		fn recursive_fib(n: i32) -> bool
		{
		    return false
		}

		fn entry() @constructor
		{
		    a: bool = recursive_fib(5)
			b := 2
			b = 3
			a = true
		}
	)");
	
	try
	{
		module->body = parser.parse();

		seam::code_generation::code_generation code_gen{ module.get() };
		auto module = code_gen.generate();
		module->print(llvm::outs(), nullptr);
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
