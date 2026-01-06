#ifndef SYCLANG_IR_IR_H
#define SYCLANG_IR_IR_H

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace syclang {

// Architecture enum
enum class Architecture {
    X64,
    ARM64
};

// Output format
enum class OutputFormat {
    ELF,
    PE,
    EFI,
    RAW
};

// IR Value types
enum class IRType {
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64,
    BOOL, VOID,
    POINTER
};

// IR Value
class IRValue {
public:
    IRValue() : type_(IRType::VOID) {}
    explicit IRValue(IRType type) : type_(type) {}
    
    IRType getType() const { return type_; }
    size_t getSize() const;
    
    virtual std::string toString() const = 0;
    virtual ~IRValue() = default;

protected:
    IRType type_;
};

// Constants
class IRConstant : public IRValue {
public:
    union {
        int64_t intValue;
        uint64_t uintValue;
        double floatValue;
    } value_;
    
    IRConstant() : IRValue() {}
    explicit IRConstant(IRType type) : IRValue(type) {}
    
    static std::shared_ptr<IRConstant> createI32(int32_t value);
    static std::shared_ptr<IRConstant> createI64(int64_t value);
    static std::shared_ptr<IRConstant> createU32(uint32_t value);
    static std::shared_ptr<IRConstant> createU64(uint64_t value);
    static std::shared_ptr<IRConstant> createF64(double value);
    static std::shared_ptr<IRConstant> createBool(bool value);
    
    std::string toString() const override;
};

// Variables
class IRVariable : public IRValue {
public:
    std::string name;
    int registerNum;
    bool isGlobal;
    int offset; // Stack offset for locals
    
    IRVariable() : IRValue() {}
    explicit IRVariable(IRType type) : IRValue(type) {}
    
    static std::shared_ptr<IRVariable> create(IRType type, const std::string& name);
    
    std::string toString() const override;
};

// Instructions
enum class Opcode {
    // Binary ops
    ADD, SUB, MUL, DIV, MOD,
    AND, OR, XOR,
    SHL, SHR,
    
    // Unary ops
    NEG, NOT, BIT_NOT,
    
    // Comparison
    EQ, NE, LT, GT, LE, GE,
    
    // Memory
    LOAD, STORE,
    ALLOCA,
    
    // Control flow
    BR, CONDBR,
    CALL, RET,
    PHI,
    
    // Conversions
    TRUNC, ZEXT, SEXT,
    FPTOUI, FPTOSI,
    UITOFP, SITOFP,
    BITCAST
};

class IRInstruction {
public:
    IRInstruction(Opcode op) : opcode(op) {}
    
    Opcode opcode;
    std::shared_ptr<IRValue> result;
    std::vector<std::shared_ptr<IRValue>> operands;
    std::string label;
    
    std::string toString() const;
};

// Basic block
class IRBasicBlock {
public:
    std::string name;
    std::vector<std::shared_ptr<IRInstruction>> instructions;
    std::shared_ptr<IRBasicBlock> nextBlock;
};

// Function
class IRFunction {
public:
    std::string name;
    IRType returnType;
    std::vector<std::pair<IRType, std::string>> parameters;
    std::vector<std::shared_ptr<IRBasicBlock>> blocks;
    int stackSize;
    bool isVariadic;
    
    void addBlock(std::shared_ptr<IRBasicBlock> block);
    std::shared_ptr<IRBasicBlock> getCurrentBlock();
};

// Module
class IRModule {
public:
    std::string name;
    std::vector<std::shared_ptr<IRFunction>> functions;
    std::vector<std::shared_ptr<IRVariable>> globalVariables;
    Architecture targetArch;
    OutputFormat outputFormat;
    
    void addFunction(std::shared_ptr<IRFunction> func);
    void addGlobalVariable(std::shared_ptr<IRVariable> var);
    
    std::string dump() const;
};

} // namespace syclang

#endif // SYCLANG_IR_IR_H
