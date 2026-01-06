cmake_minimum_required(VERSION 3.15)
project(SysLang VERSION 4.0.0 LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

# Source files - 只包含实际存在的文件
set(LIB_SOURCES
    # Core compiler
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/lexer/chinese_lexer.cpp
    src/parser/parser.cpp
    src/parser/ast.cpp
    src/ir/ir_generator.cpp
    src/ir/ir.cpp
    src/ir/platform_generator.cpp
    src/ir/actor_system.cpp
    
    # Code generation
    src/codegen/codegen_base.cpp
    src/codegen/arm64/arm64_codegen.cpp
    src/codegen/x64/x64_codegen.cpp
    src/codegen/inline_assembly.cpp
    
    # Optimization
    src/optimizer/optimizer.cpp
    
    # Utilities
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

# Install targets
install(TARGETS syclang DESTINATION bin)
install(DIRECTORY examples/ DESTINATION share/syclang/examples)
install(FILES README.md LICENSE CONTRIBUTING.md DESTINATION share/syclang)

# Configuration options
option(BUILD_V3_FEATURES "Build SysLang v3.0 features" ON)
option(BUILD_V4_FEATURES "Build SysLang v4.0 features" ON)

if(BUILD_V3_FEATURES)
    add_compile_definitions(SYSLANG_V3_ENABLED)
endif()

if(BUILD_V4_FEATURES)
    add_compile_definitions(SYSLANG_V4_ENABLED)
endif()
