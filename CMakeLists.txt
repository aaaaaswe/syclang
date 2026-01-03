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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()

# Source files
set(LIB_SOURCES
    # Core compiler
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/lexer/chinese_lexer.cpp
    src/parser/parser.cpp
    src/parser/ast.cpp
    src/parser/chinese_validation.cpp
    src/ir/ir_generator.cpp
    src/ir/ir.cpp
    src/ir/platform_generator.cpp
    src/ir/actor_system.cpp
    
    # Code generation
    src/codegen/codegen_base.cpp
    src/codegen/arm64/arm64_codegen.cpp
    src/codegen/x64/x64_codegen.cpp
    src/codegen/x86/x86_codegen.cpp
    src/codegen/macos/macos_codegen.cpp
    src/codegen/wasm_codegen.cpp
    src/codegen/inline_assembly.cpp
    src/codegen/quantum_codegen.cpp
    src/codegen/fpga_codegen.cpp
    
    # Optimization
    src/optimizer/optimizer.cpp
    src/optimizer/wasm_optimizer.cpp
    src/optimizer/quantum_optimizer.cpp
    
    # Verification
    src/verification/model_checker.cpp
    src/verification/theorem_prover.cpp
    src/verification/symbolic_executor.cpp
    src/verification/smt_solver.cpp
    src/verification/invariant_checker.cpp
    
    # AI Assistant
    src/ai/ai_assistant.cpp
    src/ai/code_analyzer.cpp
    src/ai/test_generator.cpp
    
    # Heterogeneous Computing
    src/hetero/compute_device.cpp
    src/hetero/compute_kernel.cpp
    src/hetero/tensor.cpp
    src/hetero/neural_network.cpp
    src/hetero/gpu_memory_pool.cpp
    src/hetero/multi_device_manager.cpp
    
    # Quantum Runtime
    src/quantum/quantum_state.cpp
    src/quantum/quantum_gate.cpp
    src/quantum/quantum_circuit.cpp
    src/quantum/quantum_simulator.cpp
    src/quantum/quantum_algorithms.cpp
    src/quantum/quantum_ml.cpp
    src/quantum/quantum_error_correction.cpp
    src/quantum/quantum_compiler.cpp
    
    # Utilities
    src/symbol_table.cpp
    src/utils/unicode_utils.cpp
    src/utils/serialization.cpp
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
install(FILES README.md LICENSE CONTRIBUTING.md DESTINATION share/syclang)

# Configuration options
option(BUILD_V3_FEATURES "Build SysLang v3.0 features (Distributed, WASM, AI)" ON)
option(BUILD_V4_FEATURES "Build SysLang v4.0 features (Quantum, Hetero, Formal)" ON)
option(BUILD_QUANTUM_SIMULATOR "Build quantum simulation support" ON)
option(BUILD_GPU_SUPPORT "Build GPU/CUDA support" OFF)
option(BUILD_FPGA_SUPPORT "Build FPGA support" OFF)
option(BUILD_AI_TOOLS "Build AI-assisted programming tools" ON)
option(BUILD_VERIFICATION "Build formal verification tools" ON)

# Conditional compilation
if(BUILD_V3_FEATURES)
    add_compile_definitions(SYSLANG_V3_ENABLED)
endif()

if(BUILD_V4_FEATURES)
    add_compile_definitions(SYSLANG_V4_ENABLED)
endif()

if(BUILD_QUANTUM_SIMULATOR)
    add_compile_definitions(SYSLANG_QUANTUM_ENABLED)
endif()

if(BUILD_GPU_SUPPORT)
    add_compile_definitions(SYSLANG_GPU_ENABLED)
    find_package(CUDA QUIET)
    if(CUDA_FOUND)
        target_link_libraries(syclang ${CUDA_LIBRARIES})
        include_directories(${CUDA_INCLUDE_DIRS})
    endif()
endif()

if(BUILD_FPGA_SUPPORT)
    add_compile_definitions(SYSLANG_FPGA_ENABLED)
endif()

if(BUILD_AI_TOOLS)
    add_compile_definitions(SYSLANG_AI_ENABLED)
    # TODO: Add AI library dependencies (e.g., libtorch, TensorFlow)
endif()

if(BUILD_VERIFICATION)
    add_compile_definitions(SYSLANG_VERIFICATION_ENABLED)
    # TODO: Add verification library dependencies (e.g., Z3, CVC5)
endif()
