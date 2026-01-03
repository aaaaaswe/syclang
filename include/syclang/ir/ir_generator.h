#ifndef SYCLANG_IR_IR_GENERATOR_H
#define SYCLANG_IR_IR_GENERATOR_H

#include "ir.h"
#include "parser/ast.h"
#include <map>
#include <memory>
#include <string>

namespace syclang {

class IRGenerator {
public:
    explicit IRGenerator(Architecture arch);
    
    // Generate IR from AST
    std::shared_ptr<IRModule> generate(std::shared_ptr<Program> program);
    
private:
    Architecture arch_;
    std::shared_ptr<IRFunction> currentFunction_;
    std::shared_ptr<IRBasicBlock> currentBlock_;
    std::map<std::string, std::shared_ptr<IRFunction>> functions_;
    std::map<std::string, std::shared_ptr<IRVariable>> variables_;
    std::map<std::string, std::shared_ptr<StructDecl>> structs_;
    
    int labelCounter_;
    int tempCounter_;
    
    // Type conversion
    IRType convertType(std::shared_ptr<Type> type);
    
    // Statement generation
    void generateFunctionBody(std::shared_ptr<FunctionDecl> func);
    void generateBlock(std::shared_ptr<BlockStmt> block);
    void generateStatement(std::shared_ptr<Statement> stmt);
    void generateLet(std::shared_ptr<LetStmt> let);
    void generateReturn(std::shared_ptr<ReturnStmt> ret);
    void generateIf(std::shared_ptr<IfStmt> ifStmt);
    void generateWhile(std::shared_ptr<WhileStmt> whileStmt);
    void generateFor(std::shared_ptr<ForStmt> forStmt);
    
    // Expression generation
    std::shared_ptr<IRValue> generateExpression(std::shared_ptr<Expression> expr);
    std::shared_ptr<IRValue> generateLiteral(std::shared_ptr<LiteralExpr> lit);
    std::shared_ptr<IRValue> generateIdentifier(std::shared_ptr<IdentifierExpr> ident);
    std::shared_ptr<IRValue> generateBinary(std::shared_ptr<BinaryExpr> binary);
    std::shared_ptr<IRValue> generateUnary(std::shared_ptr<UnaryExpr> unary);
    std::shared_ptr<IRValue> generateCall(std::shared_ptr<CallExpr> call);
    std::shared_ptr<IRValue> generateCast(std::shared_ptr<CastExpr> cast);
    std::shared_ptr<IRValue> generateIndex(std::shared_ptr<IndexExpr> index);
    std::shared_ptr<IRValue> generateMemberAccess(std::shared_ptr<MemberAccessExpr> access);
    std::shared_ptr<IRValue> generateAsm(std::shared_ptr<AsmExpr> asmExpr);
    
    // Helper functions
    std::shared_ptr<IRVariable> newTemp();
    std::string newLabel(const std::string& prefix);
};

} // namespace syclang

#endif // SYCLANG_IR_IR_GENERATOR_H
