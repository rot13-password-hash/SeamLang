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
type a = i32
type a = i32
	)");
	
	try
	{
		module->body = parser.parse();

		//seam::code_generation::code_generation code_gen{ module.get() };
		//code_gen.generate();
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
