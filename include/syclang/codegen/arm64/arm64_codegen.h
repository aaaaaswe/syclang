#ifndef SYCLANG_CODEGEN_ARM64_ARM64_CODEGEN_H
#define SYCLANG_CODEGEN_ARM64_ARM64_CODEGEN_H

#include "syclang/codegen/codegen_base.h"
#include <string>

namespace syclang {

class ARM64CodeGenerator : public CodeGenerator {
public:
    ARM64CodeGenerator();
    
    void generate(std::shared_ptr<IRModule> module) override;
    std::string getOutput() const override { return output_; }
    
private:
    std::string output_;
    
    void emitPrologue(const std::string& funcName) override;
    void emitEpilogue(const std::string& funcName) override;
    void emitInstruction(std::shared_ptr<IRInstruction> inst) override;
    
    std::string getReturnValueRegister() override { return "x0"; }
    std::string getStackPointerRegister() override { return "sp"; }
    std::string getFramePointerRegister() override { return "x29"; }
    
    // ARM64 specific
    std::string valueToOperand(std::shared_ptr<IRValue> value);
    std::string emitBinaryOp(Opcode op, std::shared_ptr<IRValue> left,
                            std::shared_ptr<IRValue> right);
    std::string emitUnaryOp(Opcode op, std::shared_ptr<IRValue> operand);
    std::string emitComparison(Opcode op);
    
    // ARM64 registers
    void initRegisters();
};

} // namespace syclang

#endif // SYCLANG_CODEGEN_ARM64_ARM64_CODEGEN_H
