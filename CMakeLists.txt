# Seam Language Compiler

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(Seam)

# Find LLVM Installation
find_package(LLVM CONFIG REQUIRED)

# Include LLVM includes and definitions
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Get rid of warnings
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)

# Create compiler executable
add_executable(compiler
	src/main.cpp
	src/seam/lexer/lexer.cpp 
	src/seam/parser/parser.cpp
	src/seam/ir/ast/statement.cpp
	src/seam/ir/ast/expression.cpp 
	src/seam/types/function_signature.hpp
	src/seam/ir/ast/type.cpp 
	src/seam/parser/passes/pass.cpp
	src/seam/parser/passes/symbol_collector.cpp
	src/seam/parser/passes/function_dependency.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(LLVM_LIBS support core irreader ${LLVM_TARGETS_TO_BUILD})

# Link against LLVM libraries
target_link_libraries(compiler ${LLVM_LIBS})

# Test Suites
add_executable(lexer_test
	src/tests/lexer_test_suite.cpp
	src/seam/lexer/lexer.cpp "src/seam/parser/passes/function_dependency.cpp")

target_link_libraries(lexer_test ${LLVM_LIBS})