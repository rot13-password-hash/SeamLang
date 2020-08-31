#include "code_generation.hpp"
#include "../ir/ast/visitor.hpp"
#include "../ir/ast/type.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

#include <iostream>
#include <variant>
#include <type_traits>

#include "../utils/exception.hpp"

namespace seam::code_generation
{
    llvm::Type* code_generation::get_llvm_type(ir::ast::resolved_type* t)
    {
        /*
        string,
        f32,
        f64*/

        return std::visit([this, t](auto&& arg) -> llvm::Type* {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, ir::ast::resolved_type::built_in_type>)
            {
                switch (arg)
                {
                    case ir::ast::resolved_type::built_in_type::void_:
                    {
                        return llvm::Type::getVoidTy(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::bool_:
                    {
                        return llvm::Type::getInt1Ty(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::u8:
                    case ir::ast::resolved_type::built_in_type::i8:
                    {
                        return llvm::Type::getInt8Ty(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::u16:
                    case ir::ast::resolved_type::built_in_type::i16:
                    {
                        return llvm::Type::getInt16Ty(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::u32:
                    case ir::ast::resolved_type::built_in_type::i32:
                    {
                        return llvm::Type::getInt32Ty(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::u64:
                    case ir::ast::resolved_type::built_in_type::i64:
                    {
                        return llvm::Type::getInt64Ty(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::string:
                    {
                        std::array<llvm::Type*, 2> fields{ size_type, llvm::PointerType::get(llvm::Type::getInt8Ty(context_), 0) };
                        return llvm::StructType::get(context_, llvm::makeArrayRef(fields), false);
                    }
                    case ir::ast::resolved_type::built_in_type::f32:
                    {
                        return llvm::Type::getFloatTy(context_);
                    }
                    case ir::ast::resolved_type::built_in_type::f64:
                    {
                        return llvm::Type::getDoubleTy(context_);
                    }
                    default:
                    {
                        throw utils::compiler_exception{ {0, 0}, "internal compiler error: unknown type" };
                    }
                }
            }
            else
            {
                throw std::runtime_error("TODO: class types are not supported");
            }
        }, t->value);
    }

    llvm::FunctionType* code_generation::get_llvm_function_type(ir::ast::statement::function_definition* func)
    {
        const auto& name = func->signature->mangled_name;
        const auto& it = function_type_map.find(name);
        if (it != function_type_map.cend())
        {
	        return it->second;
        }
    	
	    // set return type
        const auto return_type = get_llvm_type(static_cast<ir::ast::resolved_type*>(func->signature->return_type->value.get()));
        if (!llvm::FunctionType::isValidReturnType(return_type))
        {
            throw utils::compiler_exception(func->range.start, "internal compiler error: invalid return type");
        }

        std::vector<llvm::Type*> param_types;
    	for (const auto& param : func->signature->parameters)
    	{
            const auto param_type = get_llvm_type(static_cast<ir::ast::resolved_type*>(param.param_type->value.get()));
    		if (!llvm::FunctionType::isValidArgumentType(param_type))
    		{
    			throw utils::compiler_exception(func->range.start, "internal compiler error: invalid parameter type");
    		}
            param_types.push_back(param_type);
    	}

        const auto function_type = llvm::FunctionType::get(return_type, llvm::makeArrayRef(param_types), false);

        function_type_map.emplace(name, function_type);
        return function_type;
    }
	
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
	    explicit code_gen_visitor(llvm::IRBuilder<>& builder, code_generation& gen)
	        : builder(builder), gen(gen) {}

        code_generation& gen;
        llvm::IRBuilder<>& builder;
        llvm::Value* value;

        bool visit(ir::ast::expression::symbol_wrapper* node) override
        {
            ;
            value = gen.get_or_declare_function(static_cast<ir::ast::expression::resolved_symbol*>(node->value.get())->signature.get());
            return false;
        }
    	
        bool visit(ir::ast::expression::call* node) override
        {
        	// TODO: Handle call expressions in code_gen.
            node->function->visit(this);
            if (!llvm::isa<llvm::Function>(value))
            {
                throw utils::compiler_exception{ node->range.start, "internal compiler error: expected function for call" };
            }

            const auto func = static_cast<llvm::Function*>(value);

            std::vector<llvm::Value*> arguments;
            for (const auto& arg : node->arguments)
            {
                arg->visit(this);
                arguments.emplace_back(value);
            }
            // TODO: More work here...
            value = builder.CreateCall(func, llvm::makeArrayRef(arguments));
            return false;
        }
    	
        bool visit(ir::ast::expression::bool_literal* node) override
        {
            value = llvm::ConstantInt::get(builder.getContext(), llvm::APInt(1, node->value));
            return false;
        }

        bool visit(ir::ast::expression::number_wrapper* node) override
        {
            const auto resolved = static_cast<ir::ast::expression::resolved_number*>(node->value.get());
            value = std::visit(
                [this, node](auto&& value) -> llvm::Value*
                {
                    using value_t = std::decay_t<decltype(value)>;
                    if constexpr (std::is_same_v<value_t, std::uint8_t>)
                    {
                        return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(sizeof(std::uint8_t) * 8, value));
                    }
                    else if constexpr (std::is_same_v<value_t, std::uint16_t>)
                    {
                        return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(sizeof(std::uint16_t) * 8, value));
                    }
                    else if constexpr (std::is_same_v<value_t, std::uint32_t>)
                    {
                        return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(sizeof(std::uint32_t) * 8, value));
                    }
                    else if constexpr (std::is_same_v<value_t, std::uint64_t>)
                    {
                        return llvm::ConstantInt::get(builder.getContext(), llvm::APInt(sizeof(std::uint64_t) * 8, value));
                    }
                    else if constexpr (std::is_same_v<value_t, float>)
                    {
                        return llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(value));
                    }
                    else if constexpr (std::is_same_v<value_t, double>)
                    {
                        return llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(value));
                    }
                    else
                    {
                        throw utils::compiler_exception{ node->range.start,
                            "internal compiler error: unknown number type" };
                    }
                }, resolved->value);
            return false;
        }

        bool visit(ir::ast::statement::while_loop* node) override
        {
            return false;
        }
    	
        bool visit(ir::ast::statement::if_stat* node) override
        {
            return false;
        }
    	
        bool visit(ir::ast::statement::ret* node) override
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

        bool visit(ir::ast::statement::block* node) override
        {
			return true;
        }

        bool visit(ir::ast::expression::binary* node) override
        {
            auto left_node = dynamic_cast<ir::ast::expression::number_wrapper*>(node->left.get());
            auto right_node = dynamic_cast<ir::ast::expression::number_wrapper*>(node->right.get());
            if (!left_node || !right_node)
            {
                throw utils::compiler_exception{
                    node->range.start,
                    "internal compiler error: binary operations for non-arithmetic types are not implemented" };
            }

            auto resolved_left = static_cast<ir::ast::expression::resolved_number*>(left_node->value.get());
            auto resolved_right = static_cast<ir::ast::expression::resolved_number*>(right_node->value.get());

            node->left->visit(this);
            const auto lhs_value = value;

            node->right->visit(this);
            const auto rhs_value = value;

            // TODO: correct?
            bool unsigned_operation = resolved_left->is_unsigned && resolved_right->is_unsigned;

            // If either left or right is floating point, use a floating point operation.
            bool float_operation = std::visit(
                [&float_operation](auto&& left_value, auto&& right_value) -> bool
                {
                    using left_value_t = std::decay_t<decltype(left_value)>;
                    using right_value_t = std::decay_t<decltype(right_value)>;
                    return (std::is_same_v<left_value_t, float> || std::is_same_v<left_value_t, double>)
                        || (std::is_same_v<right_value_t, float> || std::is_same_v<right_value_t, double>);
                }, resolved_left->value, resolved_right->value);

            switch (node->operation)
            {
                case lexer::lexeme_type::symbol_add:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFAdd(lhs_value, rhs_value, "faddtmp");
                    }
                    else
                    {
                        value = builder.CreateAdd(lhs_value, rhs_value, "addtmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_minus:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFSub(lhs_value, rhs_value, "fsubtmp");
                    }
                    else
                    {
                        value = builder.CreateSub(lhs_value, rhs_value, "subtmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_multiply:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFMul(lhs_value, rhs_value, "fmultmp");
                    }
                    else
                    {
                        value = builder.CreateMul(lhs_value, rhs_value, "multmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_divide:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFDiv(lhs_value, rhs_value, "fdivtmp");
                    }
                    else if (unsigned_operation)
                    {
                        value = builder.CreateUDiv(lhs_value, rhs_value, "udivtmp");
                    }
                    else
                    {
                        value = builder.CreateSDiv(lhs_value, rhs_value, "sdivtmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_eq:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpOEQ(lhs_value, rhs_value, "feqtmp");
                    }
                    else
                    {
                        value = builder.CreateICmpEQ(lhs_value, rhs_value, "eqtmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_neq:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpONE(lhs_value, rhs_value, "fnetmp");
                    }
                    else
                    {
                        value = builder.CreateICmpNE(lhs_value, rhs_value, "netmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_lt:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpOLT(lhs_value, rhs_value, "folttmp");
                    }
                    else if (unsigned_operation)
                    {
                        value = builder.CreateICmpULT(lhs_value, rhs_value, "ulttmp");
                    }
                    else
                    {
                        value = builder.CreateICmpSLT(lhs_value, rhs_value, "slttmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_lteq:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpOLE(lhs_value, rhs_value, "foletmp");
                    }
                    else if (unsigned_operation)
                    {
                        value = builder.CreateICmpSLE(lhs_value, rhs_value, "uletmp");
                    }
                    else
                    {
                        value = builder.CreateICmpSLE(lhs_value, rhs_value, "sletmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_gt:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpOGT(lhs_value, rhs_value, "fogttmp");
                    }
                    else if (unsigned_operation)
                    {
                        value = builder.CreateICmpUGT(lhs_value, rhs_value, "ugttmp");
                    }
                    else
                    {
                        value = builder.CreateICmpSGT(lhs_value, rhs_value, "sgttmp");
                    }
                    break;
                }
                case lexer::lexeme_type::symbol_gteq:
                {
                    if (float_operation)
                    {
                        value = builder.CreateFCmpOGE(lhs_value, rhs_value, "fogetmp");
                    }
                    else if (unsigned_operation)
                    {
                        value = builder.CreateICmpSGE(lhs_value, rhs_value, "ugetmp");
                    }
                    else
                    {
                        value = builder.CreateICmpSGE(lhs_value, rhs_value, "sgetmp");
                    }
                    break;
                }
                default:
                {
                    throw utils::compiler_exception { node->range.start, "internal compiler error: invalid binary operation" };
                }
            }

			return false;
        }
    };
	
    llvm::Function* code_generation::get_or_declare_function(ir::ast::function_signature* signature)
    {
        auto func = llvm_module->getFunction(signature->mangled_name);
    	if (!func)
    	{
            //llvm::FunctionType* func_type = get_llvm_function_type(def_stat);
            //func = llvm::Function::Create(func_type, llvm::GlobalValue::InternalLinkage, def_stat->signature->mangled_name, *llvm_module);
    	}
        return func;
    }

    void code_generation::compile_function(ir::ast::statement::function_definition* func)
	{
        llvm::Function* llvm_func = llvm::Function::Create(get_llvm_function_type(func),
            llvm::GlobalValue::InternalLinkage,  func->signature->mangled_name, *llvm_module);
        llvm::BasicBlock* basic_block = llvm::BasicBlock::Create(context_, "entry",
            llvm_func);
        llvm::IRBuilder<> builder(basic_block);

        code_gen_visitor code_gen { builder, *this };
        func->body->visit(&code_gen);

        const auto& attribs = func->signature->attributes;
        if (attribs.find("constructor") != attribs.cend())
        {
            builder.CreateRetVoid(); //TODO: remove once types are added
            constructor_functions.push_back(llvm_func);
        }

        std::string error;
        llvm::raw_string_ostream error_stream{ error };
        if (llvm::verifyFunction(*llvm_func, &error_stream))
        {
            throw std::runtime_error(error);
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

        auto entry_function = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(context_), false),
            llvm::GlobalValue::InternalLinkage,
            "entry", 
            *llvm_module);
		auto entry_basic_block = llvm::BasicBlock::Create(context_, "entry", entry_function);
        llvm::IRBuilder<> entry_builder(entry_basic_block);

        for (const auto& constructor_func : constructor_functions)
        {
            entry_builder.CreateCall(constructor_func);
        }

        entry_builder.CreateRetVoid();

        std::string error;
        llvm::raw_string_ostream error_stream{ error };
        if (llvm::verifyModule(*llvm_module, &error_stream))
        {
	        throw std::runtime_error(error);
        }
		
        return llvm_module;
    }
}
