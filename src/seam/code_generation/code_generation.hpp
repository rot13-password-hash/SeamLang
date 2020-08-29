#pragma once 

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include "../ir/ast/statement.hpp"
#include "../types/module.hpp"

#include <memory>

namespace seam::code_generation
{
    class code_generation
    {
        llvm::LLVMContext context_;

        std::shared_ptr<llvm::Module> llvm_module;
        

        types::module* mod_;

        std::vector<llvm::Function*> constructor_functions;

		void compile_function(ir::ast::statement::function_definition* func);
    public:
        code_generation(types::module* mod) :
			llvm_module(std::make_shared<llvm::Module>(mod->name, context_)), mod_(mod) {}

        std::shared_ptr<llvm::Module> generate();
    };
}