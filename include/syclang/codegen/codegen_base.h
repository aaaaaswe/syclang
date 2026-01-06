#ifndef SYCLANG_CODEGEN_CODEGEN_BASE_H
#define SYCLANG_CODEGEN_CODEGEN_BASE_H

#include "syclang/ir/ir.h"
#include <memory>
#include <string>
#include <vector>

namespace syclang {

class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    virtual void generate(std::shared_ptr<IRModule> module) = 0;
    virtual std::string getOutput() const = 0;
    
    Architecture getArchitecture() const { return arch_; }
    
protected:
    Architecture arch_;
    
    // Register allocation
    struct RegisterInfo {
        std::string name;
        bool isCallerSave;
        int size;
    };
    std::vector<RegisterInfo> registers_;
    
    // Stack management
    int currentStackOffset_;
    
    // Helper methods
    virtual void emitPrologue(const std::string& funcName) = 0;
    virtual void emitEpilogue(const std::string& funcName) = 0;
    virtual void emitInstruction(std::shared_ptr<IRInstruction> inst) = 0;
    
    virtual std::string getReturnValueRegister() = 0;
    virtual std::string getStackPointerRegister() = 0;
    virtual std::string getFramePointerRegister() = 0;
};

} // namespace syclang

#endif // SYCLANG_CODEGEN_CODEGEN_BASE_H
