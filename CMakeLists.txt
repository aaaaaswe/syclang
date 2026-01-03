cmake_minimum_required(VERSION 3.15)
project(SysLang VERSION 1.0.0 LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()

# Source files
set(LIB_SOURCES
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/parser/parser.cpp
    src/parser/ast.cpp
    src/ir/ir_generator.cpp
    src/ir/ir.cpp
    src/codegen/codegen_base.cpp
    src/codegen/arm64/arm64_codegen.cpp
    src/codegen/x64/x64_codegen.cpp
    src/codegen/inline_assembly.cpp
    src/optimizer/optimizer.cpp
    src/symbol_table.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Create static library
add_library(syclang_lib STATIC ${LIB_SOURCES})

# Create main executable
add_executable(syclang ${MAIN_SOURCES})
target_link_libraries(syclang syclang_lib)

# Runtime library
add_subdirectory(lib)

# Tests
enable_testing()
add_subdirectory(tests)

# Install targets
install(TARGETS syclang DESTINATION bin)
install(DIRECTORY lib/ DESTINATION lib/syclang)
install(DIRECTORY examples/ DESTINATION share/syclang/examples)
