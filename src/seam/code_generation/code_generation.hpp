#pragma once 

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include "../ir/ast/statement.hpp"
#include "../types/module.hpp"

#include <memory>
#include <unordered_map>

namespace seam::code_generation
{
    class code_generation
    {
        llvm::LLVMContext context_;

        std::shared_ptr<llvm::Module> llvm_module;
        std::unique_ptr<llvm::DataLayout> data_layout;

        types::module* mod_;

        std::vector<llvm::Function*> constructor_functions;
        std::unordered_map<std::string, llvm::FunctionType*> function_type_map;
    	
        llvm::Type* size_type;

        
        llvm::FunctionType* get_llvm_function_type(utils::position position, ir::ast::expression::function_signature* signature);

        
    	void compile_function(ir::ast::statement::function_definition* func);
        void compile_extern_function(ir::ast::statement::extern_function_definition* func);

    public:
        code_generation(types::module* mod) :
			llvm_module(std::make_shared<llvm::Module>(mod->name, context_)),
    		data_layout(std::make_unique<llvm::DataLayout>(llvm_module.get())),
    		mod_(mod),
            size_type(llvm::Type::getIntNTy(context_, data_layout->getMaxPointerSizeInBits()))
        {}

        llvm::Type* get_llvm_type(ir::ast::type* t);
        std::shared_ptr<llvm::Module> generate();
    	llvm::Function* get_or_declare_function(utils::position position, ir::ast::expression::function_signature* signature);
    };
}