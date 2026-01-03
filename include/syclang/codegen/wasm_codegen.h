/**
 * @file wasm_codegen.h
 * @brief WebAssembly 代码生成器 - SysLang v3.0
 * 
 * 支持 WebAssembly (WASM) 编译目标
 * 支持 JavaScript 互操作
 * 支持浏览器和 Node.js 环境
 * 
 * SysLang v3.0 新增功能：WebAssembly 后端
 */

#ifndef SYCLANG_CODEGEN_WASM_CODEGEN_H
#define SYCLANG_CODEGEN_WASM_CODEGEN_H

#include "syclang/codegen/codegen_base.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace syclang {
namespace codegen {

/**
 * @brief WebAssembly 模块配置
 */
struct WasmModuleConfig {
    // 内存配置
    size_t memory_limit;       // 内存限制（字节）
    size_t heap_size;          // 堆大小（字节）
    bool enable_shared_memory; // 是否启用共享内存
    
    // 编译优化
    int optimization_level;    // 优化级别（0-3）
    bool enable_simd;          // 是否启用 SIMD
    bool enable_threads;       // 是否启用多线程
    
    // JavaScript 互操作
    bool enable_js_interop;    // 启用 JS 互操作
    bool import_js_functions;  // 导入 JS 函数
    bool export_to_js;         // 导出函数到 JS
    
    // 目标环境
    enum class TargetEnvironment {
        BROWSER,
        NODEJS,
        WASI
    };
    TargetEnvironment target_env;
    
    WasmModuleConfig()
        : memory_limit(16 * 1024 * 1024),
          heap_size(8 * 1024 * 1024),
          enable_shared_memory(false),
          optimization_level(3),
          enable_simd(true),
          enable_threads(false),
          enable_js_interop(true),
          import_js_functions(true),
          export_to_js(true),
          target_env(TargetEnvironment::BROWSER) {}
};

/**
 * @brief WebAssembly 类型定义
 */
enum class WasmType {
    I32,
    I64,
    F32,
    F64,
    V128,
    FUNCREF,
    EXTERNREF
};

/**
 * @brief WebAssembly 导出/导入描述符
 */
struct WasmExport {
    std::string name;
    WasmType return_type;
    std::vector<WasmType> param_types;
    bool exported_to_js;
};

struct WasmImport {
    std::string module;
    std::string name;
    WasmType return_type;
    std::vector<WasmType> param_types;
};

/**
 * @brief WebAssembly 指令
 */
enum class WasmOpcode {
    // 控制指令
    NOP,
    UNREACHABLE,
    BLOCK,
    LOOP,
    IF,
    ELSE,
    END,
    BR,
    BR_IF,
    BR_TABLE,
    RETURN,
    CALL,
    CALL_INDIRECT,
    DROP,
    SELECT,
    
    // 参数指令
    LOCAL_GET,
    LOCAL_SET,
    LOCAL_TEE,
    GLOBAL_GET,
    GLOBAL_SET,
    
    // 内存指令
    I32_LOAD,
    I64_LOAD,
    F32_LOAD,
    F64_LOAD,
    I32_STORE,
    I64_STORE,
    F32_STORE,
    F64_STORE,
    MEMORY_SIZE,
    MEMORY_GROW,
    
    // 算术指令
    I32_ADD,
    I32_SUB,
    I32_MUL,
    I32_DIV_S,
    I32_DIV_U,
    I32_REM_S,
    I32_REM_U,
    I32_AND,
    I32_OR,
    I32_XOR,
    I32_SHL,
    I32_SHR_S,
    I32_SHR_U,
    I32_ROTL,
    I32_ROTR,
    
    // 比较指令
    I32_EQ,
    I32_NE,
    I32_LT_S,
    I32_LT_U,
    I32_GT_S,
    I32_GT_U,
    I32_LE_S,
    I32_LE_U,
    I32_GE_S,
    I32_GE_U,
    
    // 转换指令
    I32_WRAP_I64,
    I64_EXTEND_I32_S,
    I64_EXTEND_I32_U,
    I32_TRUNC_F32_S,
    I32_TRUNC_F32_U,
    I32_TRUNC_F64_S,
    I32_TRUNC_F64_U,
    
    // 常量
    I32_CONST,
    I64_CONST,
    F32_CONST,
    F64_CONST
};

/**
 * @brief WebAssembly 代码生成器
 */
class WasmCodegen : public CodegenBase {
public:
    WasmCodegen(const WasmModuleConfig& config = WasmModuleConfig());
    virtual ~WasmCodegen();
    
    // 实现基类接口
    void generate(const ir::Module& module, std::ostream& output) override;
    Target get_target() const override { return Target::WASM; }
    
    // WASM 特定接口
    void add_export(const WasmExport& export_desc);
    void add_import(const WasmImport& import_desc);
    
    void set_function_table(const std::vector<std::string>& functions);
    void add_function(const std::string& name, const std::vector<WasmOpcode>& body);
    
    void set_memory_size(size_t size);
    void enable_features(bool simd, bool threads, bool shared_memory);
    
private:
    // 生成 WASM 模块
    void generate_module_header(std::ostream& output);
    void generate_type_section(std::ostream& output);
    void generate_import_section(std::ostream& output);
    void generate_function_section(std::ostream& output);
    void generate_table_section(std::ostream& output);
    void generate_memory_section(std::ostream& output);
    void generate_global_section(std::ostream& output);
    void generate_export_section(std::ostream& output);
    void generate_start_section(std::ostream& output);
    void generate_element_section(std::ostream& output);
    void generate_code_section(std::ostream& output);
    void generate_data_section(std::ostream& output);
    
    // 辅助函数
    WasmType ir_type_to_wasm(const ir::Type& type);
    WasmOpcode ir_op_to_wasm(const ir::Instruction& instr);
    
    void write_u8(std::ostream& output, uint8_t value);
    void write_u32(std::ostream& output, uint32_t value);
    void write_u64(std::ostream& output, uint64_t value);
    void write_bytes(std::ostream& output, const std::vector<uint8_t>& bytes);
    void write_string(std::ostream& output, const std::string& str);
    
    // 生成 WASM 二进制格式的 JavaScript 包装器
    void generate_js_wrapper(std::ostream& output, const std::string& wasm_filename);
    
    // 配置
    WasmModuleConfig config_;
    
    // 导出/导入
    std::vector<WasmExport> exports_;
    std::vector<WasmImport> imports_;
    
    // 函数定义
    struct WasmFunction {
        std::string name;
        WasmType return_type;
        std::vector<WasmType> param_types;
        std::vector<WasmOpcode> body;
        std::vector<uint8_t> locals;
    };
    std::vector<WasmFunction> functions_;
    
    // 全局变量
    struct WasmGlobal {
        std::string name;
        WasmType type;
        bool mutable_;
        std::vector<WasmOpcode> init;
    };
    std::vector<WasmGlobal> globals_;
    
    // 内存配置
    size_t memory_pages_;
    
    // 函数表
    std::vector<std::string> function_table_;
};

/**
 * @brief WebAssembly JavaScript 互操作工具
 */
class WasmJSInterop {
public:
    // 生成 JavaScript 调用包装器
    static std::string generate_js_wrapper(const std::string& wasm_module,
                                           const std::vector<WasmExport>& exports);
    
    // 生成 JavaScript 回调绑定
    static std::string generate_js_callback_binding(const std::string& func_name,
                                                    const WasmExport& export_desc);
    
    // 生成 Node.js 加载代码
    static std::string generate_node_loader(const std::string& wasm_path);
    
    // 生成浏览器加载代码
    static std::string generate_browser_loader(const std::string& wasm_url);
};

/**
 * @brief WebAssembly 优化器
 */
class WasmOptimizer {
public:
    // 优化 WASM 模块
    static std::vector<uint8_t> optimize(const std::vector<uint8_t>& wasm_binary,
                                        int level);
    
    // 内联优化
    static void inline_functions(std::vector<uint8_t>& wasm_binary);
    
    // 常量折叠
    static void constant_folding(std::vector<uint8_t>& wasm_binary);
    
    // 死代码消除
    static void dead_code_elimination(std::vector<uint8_t>& wasm_binary);
    
    // 循环展开
    static void loop_unrolling(std::vector<uint8_t>& wasm_binary);
};

} // namespace codegen
} // namespace syclang

#endif // SYCLANG_CODEGEN_WASM_CODEGEN_H
