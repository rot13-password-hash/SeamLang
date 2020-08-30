#pragma once 

#include <memory>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include "../ir/ast/statement.hpp"

namespace seam::code_generation
{
    class code_generation
    {
        llvm::LLVMContext context_;

        std::unique_ptr<ir::ast::statement::restricted_block> root_;
    public:
        code_generation(std::unique_ptr<ir::ast::statement::restricted_block> root) :
            root_(std::move(root)) {}

        std::unique_ptr<llvm::Module> generate();
    };
}