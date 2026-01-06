#include "syclang/ir/ir.h"
#include <sstream>

namespace syclang {

// IRType 输出操作符
std::ostream& operator<<(std::ostream& os, IRType type) {
    switch (type) {
        case IRType::I8: os << "i8"; break;
        case IRType::I16: os << "i16"; break;
        case IRType::I32: os << "i32"; break;
        case IRType::I64: os << "i64"; break;
        case IRType::U8: os << "u8"; break;
        case IRType::U16: os << "u16"; break;
        case IRType::U32: os << "u32"; break;
        case IRType::U64: os << "u64"; break;
        case IRType::F32: os << "f32"; break;
        case IRType::F64: os << "f64"; break;
        case IRType::BOOL: os << "bool"; break;
        case IRType::VOID: os << "void"; break;
        case IRType::POINTER: os << "ptr"; break;
        default: os << "unknown"; break;
    }
    return os;
}

size_t IRValue::getSize() const {
    switch (type_) {
        case IRType::I8:
        case IRType::U8:
        case IRType::BOOL:
            return 1;
        case IRType::I16:
        case IRType::U16:
            return 2;
        case IRType::I32:
        case IRType::U32:
        case IRType::F32:
            return 4;
        case IRType::I64:
        case IRType::U64:
        case IRType::F64:
        case IRType::POINTER:
            return 8;
        case IRType::VOID:
            return 0;
    }
    return 0;
}

std::shared_ptr<IRConstant> IRConstant::createI32(int32_t value) {
    auto constant = std::make_shared<IRConstant>(IRType::I32);
    constant->value_.intValue = value;
    return constant;
}

std::shared_ptr<IRConstant> IRConstant::createI64(int64_t value) {
    auto constant = std::make_shared<IRConstant>(IRType::I64);
    constant->value_.intValue = value;
    return constant;
}

std::shared_ptr<IRConstant> IRConstant::createU32(uint32_t value) {
    auto constant = std::make_shared<IRConstant>(IRType::U32);
    constant->value_.uintValue = value;
    return constant;
}

std::shared_ptr<IRConstant> IRConstant::createU64(uint64_t value) {
    auto constant = std::make_shared<IRConstant>(IRType::U64);
    constant->value_.uintValue = value;
    return constant;
}

std::shared_ptr<IRConstant> IRConstant::createF64(double value) {
    auto constant = std::make_shared<IRConstant>(IRType::F64);
    constant->value_.floatValue = value;
    return constant;
}

std::shared_ptr<IRConstant> IRConstant::createBool(bool value) {
    auto constant = std::make_shared<IRConstant>(IRType::BOOL);
    constant->value_.intValue = value ? 1 : 0;
    return constant;
}

std::string IRConstant::toString() const {
    std::stringstream ss;
    switch (type_) {
        case IRType::I8:
        case IRType::I16:
        case IRType::I32:
        case IRType::I64:
            ss << value_.intValue;
            break;
        case IRType::U8:
        case IRType::U16:
        case IRType::U32:
        case IRType::U64:
            ss << value_.uintValue;
            break;
        case IRType::F32:
        case IRType::F64:
            ss << value_.floatValue;
            break;
        case IRType::BOOL:
            ss << (value_.intValue ? "true" : "false");
            break;
        default:
            ss << "unknown";
    }
    return ss.str();
}

std::shared_ptr<IRVariable> IRVariable::create(IRType type, const std::string& name) {
    auto var = std::make_shared<IRVariable>(type);
    var->name = name;
    var->registerNum = -1;
    var->isGlobal = false;
    var->offset = 0;
    return var;
}

std::string IRVariable::toString() const {
    if (isGlobal) {
        return "@" + name;
    }
    return "%" + name;
}

std::string IRInstruction::toString() const {
    std::stringstream ss;
    
    switch (opcode) {
        case Opcode::ADD: ss << "add"; break;
        case Opcode::SUB: ss << "sub"; break;
        case Opcode::MUL: ss << "mul"; break;
        case Opcode::DIV: ss << "div"; break;
        case Opcode::MOD: ss << "mod"; break;
        case Opcode::AND: ss << "and"; break;
        case Opcode::OR: ss << "or"; break;
        case Opcode::XOR: ss << "xor"; break;
        case Opcode::SHL: ss << "shl"; break;
        case Opcode::SHR: ss << "shr"; break;
        case Opcode::NEG: ss << "neg"; break;
        case Opcode::NOT: ss << "not"; break;
        case Opcode::BIT_NOT: ss << "bit_not"; break;
        case Opcode::EQ: ss << "eq"; break;
        case Opcode::NE: ss << "ne"; break;
        case Opcode::LT: ss << "lt"; break;
        case Opcode::GT: ss << "gt"; break;
        case Opcode::LE: ss << "le"; break;
        case Opcode::GE: ss << "ge"; break;
        case Opcode::LOAD: ss << "load"; break;
        case Opcode::STORE: ss << "store"; break;
        case Opcode::ALLOCA: ss << "alloca"; break;
        case Opcode::BR: ss << "br"; break;
        case Opcode::CONDBR: ss << "condbr"; break;
        case Opcode::CALL: ss << "call"; break;
        case Opcode::RET: ss << "ret"; break;
        case Opcode::PHI: ss << "phi"; break;
        case Opcode::TRUNC: ss << "trunc"; break;
        case Opcode::ZEXT: ss << "zext"; break;
        case Opcode::SEXT: ss << "sext"; break;
        case Opcode::FPTOUI: ss << "fptoui"; break;
        case Opcode::FPTOSI: ss << "fptosi"; break;
        case Opcode::UITOFP: ss << "uitofp"; break;
        case Opcode::SITOFP: ss << "sitofp"; break;
        case Opcode::BITCAST: ss << "bitcast"; break;
    }
    
    ss << " ";
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << operands[i]->toString();
    }
    
    return ss.str();
}

void IRFunction::addBlock(std::shared_ptr<IRBasicBlock> block) {
    blocks.push_back(block);
}

std::shared_ptr<IRBasicBlock> IRFunction::getCurrentBlock() {
    if (blocks.empty()) {
        return nullptr;
    }
    return blocks.back();
}

void IRModule::addFunction(std::shared_ptr<IRFunction> func) {
    functions.push_back(func);
}

void IRModule::addGlobalVariable(std::shared_ptr<IRVariable> var) {
    globalVariables.push_back(var);
}

std::string IRModule::dump() const {
    std::stringstream ss;
    
    // Target triple
    ss << "target triple = \"";
    if (targetArch == Architecture::X64) {
        ss << "x86_64-unknown-linux-gnu";
    } else {
        ss << "aarch64-unknown-linux-gnu";
    }
    ss << "\"\n\n";
    
    // Global variables
    for (const auto& var : globalVariables) {
        ss << var->toString() << " = global ";
        // Emit type...
        ss << " zeroinitializer\n";
    }
    
    ss << "\n";
    
    // Functions
    for (const auto& func : functions) {
        ss << "define " << func->name << "(";
        for (size_t i = 0; i < func->parameters.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << func->parameters[i].second << " %" << func->parameters[i].first;
        }
        ss << ") {\n";
        
        for (const auto& block : func->blocks) {
            ss << block->name << ":\n";
            for (const auto& inst : block->instructions) {
                ss << "  " << inst->toString() << "\n";
            }
        }
        
        ss << "}\n\n";
    }
    
    return ss.str();
}

} // namespace syclang
