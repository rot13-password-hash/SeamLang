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
	src/lexer/lexer.cpp 
	src/parser/parser.cpp
	src/ir/ast/statement.cpp
	src/utils/error.cpp
	src/ir/ast/expression.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(LLVM_LIBS support core irreader ${LLVM_TARGETS_TO_BUILD})

# Link against LLVM libraries
target_link_libraries(compiler ${LLVM_LIBS})

# Test Suites
add_executable(lexer_test
	src/tests/lexer_test_suite.cpp
	src/lexer/lexer.cpp)

target_link_libraries(lexer_test ${LLVM_LIBS})