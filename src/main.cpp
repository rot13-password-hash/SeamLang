#include <llvm/Support/WithColor.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/Program.h>

#include "seam/parser/parser.hpp"
#include "seam/utils/exception.hpp"
#include "seam/types/module.hpp"
#include "seam/code_generation/code_generation.hpp"

#include <memory>

int main()
{
	const auto module = std::make_shared<seam::types::module>("test_module");

	seam::parser::parser parser(module, "test", R"(
fn main()
{
	x := 5 + 3
	return
}
	)");
	
	try
	{
		module->body = parser.parse();

		/*seam::code_generation::code_generation code_gen{ module.get() };
		auto module = code_gen.generate();
		module->print(llvm::outs(), nullptr);

		std::error_code error_code;
		llvm::raw_fd_ostream llvm_bitcode {"output.bc", error_code };

		llvm::WriteBitcodeToFile(*module, llvm_bitcode);
		llvm_bitcode.close();

		std::string llc_executable{ "C:\\Program Files (x86)\\llvm\\bin\\llc" };

		std::vector<llvm::StringRef> llc_args;
		llc_args.push_back(llc_executable);

		llc_args.push_back("-o");
		llc_args.push_back("output.obj");

		llc_args.push_back("-filetype=obj");

		llc_args.push_back("output.bc");

		auto status = llvm::sys::ExecuteAndWait(llc_executable, llvm::makeArrayRef(llc_args), llvm::None);
		if (status == -1)
		{
			throw std::runtime_error("bitcode compiler failed to execute, make sure you have llc in the same folder as the compiler");
		}
		else if (status == -2)
		{
			throw std::runtime_error("crash during bitcode compiling process");
		}*/
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
