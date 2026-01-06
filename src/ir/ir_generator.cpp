#include "syclang/ir/ir_generator.h"
#include "syclang/parser/ast.h"
#include <iostream>

namespace syclang {

IRGenerator::IRGenerator(Architecture arch)
    : arch_(arch), currentFunction_(nullptr), currentBlock_(nullptr),
      labelCounter_(0), tempCounter_(0) {}

std::shared_ptr<IRModule> IRGenerator::generate(std::shared_ptr<Program> program) {
    auto module = std::make_shared<IRModule>();
    module->name = "module";
    module->targetArch = arch_;
    
    // First pass: collect all function declarations
    for (const auto& decl : program->declarations) {
        auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(decl);
        if (funcDecl) {
            auto func = std::make_shared<IRFunction>();
            func->name = funcDecl->name;
            func->isVariadic = funcDecl->isVariadic;
            
            // Convert return type
            func->returnType = convertType(funcDecl->returnType);
            
            // Convert parameters
            for (const auto& param : funcDecl->params) {
                IRType paramType = convertType(param.second);
                func->parameters.push_back({paramType, param.first});
            }
            
            module->addFunction(func);
            functions_[funcDecl->name] = func;
        }
        
        auto structDecl = std::dynamic_pointer_cast<StructDecl>(decl);
        if (structDecl) {
            structs_[structDecl->name] = structDecl;
        }
    }
    
    // Second pass: generate function bodies
    for (const auto& decl : program->declarations) {
        auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(decl);
        if (funcDecl && !funcDecl->isExtern && funcDecl->body) {
            currentFunction_ = functions_[funcDecl->name];
            generateFunctionBody(funcDecl);
        }
    }
    
    currentFunction_ = nullptr;
    return module;
}

IRType IRGenerator::convertType(std::shared_ptr<Type> type) {
    if (!type) {
        return IRType::VOID;
    }
    
    switch (type->category) {
        case TypeCategory::I8: return IRType::I8;
        case TypeCategory::I16: return IRType::I16;
        case TypeCategory::I32: return IRType::I32;
        case TypeCategory::I64: return IRType::I64;
        case TypeCategory::U8: return IRType::U8;
        case TypeCategory::U16: return IRType::U16;
        case TypeCategory::U32: return IRType::U32;
        case TypeCategory::U64: return IRType::U64;
        case TypeCategory::F32: return IRType::F32;
        case TypeCategory::F64: return IRType::F64;
        case TypeCategory::BOOL: return IRType::BOOL;
        case TypeCategory::VOID: return IRType::VOID;
        case TypeCategory::POINTER:
        case TypeCategory::ARRAY:
        case TypeCategory::STRUCT:
        case TypeCategory::ENUM:
        case TypeCategory::FUNCTION:
            return IRType::POINTER;
    }
    return IRType::VOID;
}

void IRGenerator::generateFunctionBody(std::shared_ptr<FunctionDecl> funcDecl) {
    // Create entry block
    auto entryBlock = std::make_shared<IRBasicBlock>();
    entryBlock->name = "entry";
    currentFunction_->addBlock(entryBlock);
    currentBlock_ = entryBlock;
    
    // Generate function body
    generateBlock(funcDecl->body);
}

void IRGenerator::generateBlock(std::shared_ptr<BlockStmt> block) {
    for (const auto& stmt : block->statements) {
        generateStatement(stmt);
    }
}

void IRGenerator::generateStatement(std::shared_ptr<Statement> stmt) {
    if (!stmt) return;
    
    if (auto let = std::dynamic_pointer_cast<LetStmt>(stmt)) {
        generateLet(let);
    } else if (auto exprStmt = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
        generateExpression(exprStmt->expr);
    } else if (auto ret = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        generateReturn(ret);
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        generateIf(ifStmt);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        generateWhile(whileStmt);
    } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
        generateFor(forStmt);
    } else if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        generateBlock(block);
    }
}

void IRGenerator::generateLet(std::shared_ptr<LetStmt> let) {
    auto var = IRVariable::create(convertType(let->type), let->name);
    var->isGlobal = false;
    var->offset = currentFunction_->stackSize;
    
    // Allocate stack space
    auto allocaInst = std::make_shared<IRInstruction>(Opcode::ALLOCA);
    allocaInst->result = var;
    currentBlock_->instructions.push_back(allocaInst);
    
    currentFunction_->stackSize += var->getSize();
    
    // Initialize if needed
    if (let->init) {
        auto value = generateExpression(let->init);
        auto storeInst = std::make_shared<IRInstruction>(Opcode::STORE);
        storeInst->operands.push_back(value);
        storeInst->operands.push_back(var);
        currentBlock_->instructions.push_back(storeInst);
    }
    
    variables_[let->name] = var;
}

void IRGenerator::generateReturn(std::shared_ptr<ReturnStmt> ret) {
    auto inst = std::make_shared<IRInstruction>(Opcode::RET);
    if (ret->expr) {
        inst->operands.push_back(generateExpression(ret->expr));
    }
    currentBlock_->instructions.push_back(inst);
}

void IRGenerator::generateIf(std::shared_ptr<IfStmt> ifStmt) {
    auto condition = generateExpression(ifStmt->condition);
    
    // Create blocks
    auto thenBlock = std::make_shared<IRBasicBlock>();
    auto elseBlock = std::make_shared<IRBasicBlock>();
    auto mergeBlock = std::make_shared<IRBasicBlock>();
    
    thenBlock->name = newLabel("then");
    elseBlock->name = newLabel("else");
    mergeBlock->name = newLabel("merge");
    
    // Conditional branch
    auto condBr = std::make_shared<IRInstruction>(Opcode::CONDBR);
    condBr->operands.push_back(condition);
    condBr->operands.push_back(IRConstant::createI64(1)); // TODO: Use proper label
    condBr->operands.push_back(IRConstant::createI64(2));
    currentBlock_->instructions.push_back(condBr);
    
    // Generate then block
    currentBlock_ = thenBlock;
    currentFunction_->addBlock(thenBlock);
    generateStatement(ifStmt->thenBranch);
    
    if (!currentBlock_->instructions.empty() && 
        currentBlock_->instructions.back()->opcode != Opcode::RET) {
        auto br = std::make_shared<IRInstruction>(Opcode::BR);
        currentBlock_->instructions.push_back(br);
    }
    
    // Generate else block if present
    if (ifStmt->elseBranch) {
        currentBlock_ = elseBlock;
        currentFunction_->addBlock(elseBlock);
        generateStatement(ifStmt->elseBranch);
        
        if (!currentBlock_->instructions.empty() && 
            currentBlock_->instructions.back()->opcode != Opcode::RET) {
            auto br = std::make_shared<IRInstruction>(Opcode::BR);
            currentBlock_->instructions.push_back(br);
        }
    }
    
    currentBlock_ = mergeBlock;
    currentFunction_->addBlock(mergeBlock);
}

void IRGenerator::generateWhile(std::shared_ptr<WhileStmt> whileStmt) {
    auto condBlock = std::make_shared<IRBasicBlock>();
    auto bodyBlock = std::make_shared<IRBasicBlock>();
    auto exitBlock = std::make_shared<IRBasicBlock>();
    
    condBlock->name = newLabel("while.cond");
    bodyBlock->name = newLabel("while.body");
    exitBlock->name = newLabel("while.exit");
    
    // Branch to condition
    auto br = std::make_shared<IRInstruction>(Opcode::BR);
    currentBlock_->instructions.push_back(br);
    
    currentFunction_->addBlock(condBlock);
    currentBlock_ = condBlock;
    
    auto condition = generateExpression(whileStmt->condition);
    
    auto condBr = std::make_shared<IRInstruction>(Opcode::CONDBR);
    condBr->operands.push_back(condition);
    condBr->operands.push_back(IRConstant::createI64(1));
    condBr->operands.push_back(IRConstant::createI64(2));
    currentBlock_->instructions.push_back(condBr);
    
    currentFunction_->addBlock(bodyBlock);
    currentBlock_ = bodyBlock;
    generateStatement(whileStmt->body);
    
    if (!currentBlock_->instructions.empty() && 
        currentBlock_->instructions.back()->opcode != Opcode::RET) {
        auto brBack = std::make_shared<IRInstruction>(Opcode::BR);
        currentBlock_->instructions.push_back(brBack);
    }
    
    currentFunction_->addBlock(exitBlock);
    currentBlock_ = exitBlock;
}

void IRGenerator::generateFor(std::shared_ptr<ForStmt> forStmt) {
    // Initialize
    if (forStmt->init) {
        generateStatement(forStmt->init);
    }
    
    auto condBlock = std::make_shared<IRBasicBlock>();
    auto bodyBlock = std::make_shared<IRBasicBlock>();
    auto updateBlock = std::make_shared<IRBasicBlock>();
    auto exitBlock = std::make_shared<IRBasicBlock>();
    
    condBlock->name = newLabel("for.cond");
    bodyBlock->name = newLabel("for.body");
    updateBlock->name = newLabel("for.update");
    exitBlock->name = newLabel("for.exit");
    
    // Branch to condition
    auto br = std::make_shared<IRInstruction>(Opcode::BR);
    currentBlock_->instructions.push_back(br);
    
    currentFunction_->addBlock(condBlock);
    currentBlock_ = condBlock;
    
    if (forStmt->condition) {
        auto condition = generateExpression(forStmt->condition);
        auto condBr = std::make_shared<IRInstruction>(Opcode::CONDBR);
        condBr->operands.push_back(condition);
        condBr->operands.push_back(IRConstant::createI64(1));
        condBr->operands.push_back(IRConstant::createI64(2));
        currentBlock_->instructions.push_back(condBr);
    }
    
    currentFunction_->addBlock(bodyBlock);
    currentBlock_ = bodyBlock;
    generateStatement(forStmt->body);
    
    if (!currentBlock_->instructions.empty() && 
        currentBlock_->instructions.back()->opcode != Opcode::RET) {
        auto br = std::make_shared<IRInstruction>(Opcode::BR);
        currentBlock_->instructions.push_back(br);
    }
    
    currentFunction_->addBlock(updateBlock);
    currentBlock_ = updateBlock;
    if (forStmt->update) {
        generateExpression(forStmt->update);
    }
    
    auto brCond = std::make_shared<IRInstruction>(Opcode::BR);
    currentBlock_->instructions.push_back(brCond);
    
    currentFunction_->addBlock(exitBlock);
    currentBlock_ = exitBlock;
}

std::shared_ptr<IRValue> IRGenerator::generateExpression(std::shared_ptr<Expression> expr) {
    if (!expr) return nullptr;
    
    if (auto lit = std::dynamic_pointer_cast<LiteralExpr>(expr)) {
        return generateLiteral(lit);
    } else if (auto ident = std::dynamic_pointer_cast<IdentifierExpr>(expr)) {
        return generateIdentifier(ident);
    } else if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        return generateBinary(binary);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        return generateUnary(unary);
    } else if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
        return generateCall(call);
    } else if (auto cast = std::dynamic_pointer_cast<CastExpr>(expr)) {
        return generateCast(cast);
    } else if (auto index = std::dynamic_pointer_cast<IndexExpr>(expr)) {
        return generateIndex(index);
    } else if (auto access = std::dynamic_pointer_cast<MemberAccessExpr>(expr)) {
        return generateMemberAccess(access);
    } else if (auto asmExpr = std::dynamic_pointer_cast<AsmExpr>(expr)) {
        return generateAsm(asmExpr);
    }
    
    return nullptr;
}

std::shared_ptr<IRValue> IRGenerator::generateLiteral(std::shared_ptr<LiteralExpr> lit) {
    switch (lit->kind) {
        case LiteralExpr::Kind::INT:
            return IRConstant::createI64(std::stoll(lit->value));
        case LiteralExpr::Kind::FLOAT:
            return IRConstant::createF64(std::stod(lit->value));
        case LiteralExpr::Kind::BOOL:
            return IRConstant::createBool(lit->value == "true");
        case LiteralExpr::Kind::STRING:
            // For now, just return a pointer
            return IRConstant::createI64(0); // TODO: String handling
        default:
            return nullptr;
    }
}

std::shared_ptr<IRValue> IRGenerator::generateIdentifier(std::shared_ptr<IdentifierExpr> ident) {
    // Look up in variables
    auto it = variables_.find(ident->name);
    if (it != variables_.end()) {
        // Load the variable
        auto loadInst = std::make_shared<IRInstruction>(Opcode::LOAD);
        loadInst->operands.push_back(it->second);
        auto result = newTemp();
        loadInst->result = result;
        currentBlock_->instructions.push_back(loadInst);
        return result;
    }
    
    // Could be a function reference
    return nullptr;
}

std::shared_ptr<IRValue> IRGenerator::generateBinary(std::shared_ptr<BinaryExpr> binary) {
    auto left = generateExpression(binary->left);
    auto right = generateExpression(binary->right);
    
    if (!left || !right) return nullptr;
    
    Opcode op;
    switch (binary->op) {
        case TokenType::PLUS: op = Opcode::ADD; break;
        case TokenType::MINUS: op = Opcode::SUB; break;
        case TokenType::STAR: op = Opcode::MUL; break;
        case TokenType::SLASH: op = Opcode::DIV; break;
        case TokenType::PERCENT: op = Opcode::MOD; break;
        case TokenType::BIT_AND: op = Opcode::AND; break;
        case TokenType::BIT_OR: op = Opcode::OR; break;
        case TokenType::BIT_XOR: op = Opcode::XOR; break;
        case TokenType::SHL: op = Opcode::SHL; break;
        case TokenType::SHR: op = Opcode::SHR; break;
        case TokenType::EQUAL_EQUAL: op = Opcode::EQ; break;
        case TokenType::NOT_EQUAL: op = Opcode::NE; break;
        case TokenType::LESS: op = Opcode::LT; break;
        case TokenType::GREATER: op = Opcode::GT; break;
        case TokenType::LESS_EQUAL: op = Opcode::LE; break;
        case TokenType::GREATER_EQUAL: op = Opcode::GE; break;
        default: return left; // Assignment handled elsewhere
    }
    
    auto inst = std::make_shared<IRInstruction>(op);
    inst->operands.push_back(left);
    inst->operands.push_back(right);
    auto result = newTemp();
    inst->result = result;
    currentBlock_->instructions.push_back(inst);
    
    return result;
}

std::shared_ptr<IRValue> IRGenerator::generateUnary(std::shared_ptr<UnaryExpr> unary) {
    auto operand = generateExpression(unary->operand);
    if (!operand) return nullptr;
    
    Opcode op;
    switch (unary->op) {
        case TokenType::MINUS: op = Opcode::NEG; break;
        case TokenType::NOT: op = Opcode::NOT; break;
        case TokenType::BIT_NOT: op = Opcode::BIT_NOT; break;
        default: return operand;
    }
    
    auto inst = std::make_shared<IRInstruction>(op);
    inst->operands.push_back(operand);
    auto result = newTemp();
    inst->result = result;
    currentBlock_->instructions.push_back(inst);
    
    return result;
}

std::shared_ptr<IRValue> IRGenerator::generateCall(std::shared_ptr<CallExpr> call) {
    std::vector<std::shared_ptr<IRValue>> args;
    for (const auto& arg : call->args) {
        args.push_back(generateExpression(arg));
    }
    
    auto inst = std::make_shared<IRInstruction>(Opcode::CALL);
    // Add function reference
    // inst->operands.push_back(...);
    for (const auto& arg : args) {
        inst->operands.push_back(arg);
    }
    
    auto result = newTemp();
    inst->result = result;
    currentBlock_->instructions.push_back(inst);
    
    return result;
}

std::shared_ptr<IRValue> IRGenerator::generateCast(std::shared_ptr<CastExpr> cast) {
    auto value = generateExpression(cast->expr);
    IRType targetType = convertType(cast->targetType);
    
    // For now, just return the value
    // TODO: Implement proper casting
    return value;
}

std::shared_ptr<IRValue> IRGenerator::generateIndex(std::shared_ptr<IndexExpr> index) {
    auto base = generateExpression(index->base);
    auto idx = generateExpression(index->index);
    
    // Load from indexed location
    auto loadInst = std::make_shared<IRInstruction>(Opcode::LOAD);
    loadInst->operands.push_back(base);
    loadInst->operands.push_back(idx);
    
    auto result = newTemp();
    loadInst->result = result;
    currentBlock_->instructions.push_back(loadInst);
    
    return result;
}

std::shared_ptr<IRValue> IRGenerator::generateMemberAccess(std::shared_ptr<MemberAccessExpr> access) {
    auto obj = generateExpression(access->object);
    
    // Calculate offset and load
    // TODO: Implement struct field access
    return obj;
}

std::shared_ptr<IRValue> IRGenerator::generateAsm(std::shared_ptr<AsmExpr> asmExpr) {
    // For now, just emit the inline assembly as a comment
    // TODO: Implement proper inline assembly support
    return nullptr;
}

std::shared_ptr<IRVariable> IRGenerator::newTemp() {
    auto temp = std::make_shared<IRVariable>(IRType::I64);
    temp->name = "t" + std::to_string(tempCounter_++);
    temp->isGlobal = false;
    return temp;
}

std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

} // namespace syclang
