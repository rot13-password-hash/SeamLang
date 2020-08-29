#include "code_generation.hpp"
#include "../ir/ast/visitor.hpp"

#include <llvm/IR/IRBuilder.h>

#include <iostream>

namespace seam::code_generation
{
	struct function_collector : ir::ast::visitor
	{
		std::vector<ir::ast::statement::function_definition*> collected_functions;

		bool visit(ir::ast::statement::function_definition* node) override
		{
			collected_functions.push_back(node);
			return false; // future: might return true for lambda funcs
		}
	};

    struct code_gen_visitor : ir::ast::visitor
    {
        llvm::IRBuilder<>& builder;
        llvm::Value* value;

        bool visit(ir::ast::statement::ret* node)
        {
            if (node->value)
            {
                node->value->visit(this); // generate return
                builder.CreateRet(value);
            }
            else
            {
                builder.CreateRetVoid();
            }
            return false;
        }

        bool visit(ir::ast::statement::block* node)
        {
			return true;
        }
    };

	void code_generation::compile_function(ir::ast::statement::function_definition* func)
	{
		llvm::FunctionType* func_type;
		if (func->signature->return_type)
		{
			func_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context_), false);
		}
		else
		{
			throw std::runtime_error("only void returns are supported");
		}

        llvm::Function* llvm_func = llvm::Function::Create(func_type, llvm::GlobalValue::InternalLinkage, 
            func->signature->mangled_name, *llvm_module);
        llvm::BasicBlock* basic_block = llvm::BasicBlock::Create(context_, func->signature->mangled_name,
            llvm_func);
        llvm::IRBuilder<> builder(basic_block);

        code_gen_visitor code_gen { builder };
        func->body->visit(&code_gen);

        auto attribs = func->signature->attributes;
        if (attribs.find("constructor") != attribs.cend())
        {
            constructor_functions.push_back(llvm_func);
        }
    }

    std::shared_ptr<llvm::Module> code_generation::generate()
    {
		function_collector collector;
		mod_->body->visit(&collector);

        // Iterate over collected functions
        for (const auto func : collector.collected_functions)
        {
			compile_function(func);
        }

        
        llvm::Function* entry_function = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(context_), false),
            llvm::GlobalValue::InternalLinkage,
            "entry", 
            *llvm_module);
        llvm::BasicBlock* entry_basic_block = llvm::BasicBlock::Create(context_, "entry", entry_function);
        llvm::IRBuilder<> entry_builder(entry_basic_block);

        for (const auto constructor_func : constructor_functions)
        {
            entry_builder.CreateCall(constructor_func);
        }

        entry_builder.CreateRetVoid();

        return llvm_module;
    }
}