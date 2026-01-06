#ifndef SYCLANG_PARSER_AST_H
#define SYCLANG_PARSER_AST_H

#include <memory>
#include <vector>
#include <string>
#include "syclang/lexer/token.h"

namespace syclang {

// Base AST node
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(class ASTVisitor& visitor) = 0;
    Token token; // Source location info
};

// Types
enum class TypeCategory {
    I8, I16, I32, I64,
    U8, U16, U32, U64,
    F32, F64,
    BOOL, VOID,
    POINTER, ARRAY, STRUCT, ENUM, FUNCTION
};

class Type {
public:
    TypeCategory category;
    std::string name;
    std::shared_ptr<Type> baseType; // For pointers and arrays
    size_t arraySize; // For arrays
    
    Type() : category(TypeCategory::VOID), arraySize(0) {}
    Type(TypeCategory cat) : category(cat), arraySize(0) {}
    
    size_t getSize() const;
    std::string toString() const;
};

// Expressions
class Expression : public ASTNode {
public:
    std::shared_ptr<Type> type;
};

class LiteralExpr : public Expression {
public:
    enum class Kind { INT, FLOAT, STRING, BOOL, CHAR };
    Kind kind;
    std::string value;
    
    void accept(ASTVisitor& visitor) override;
};

class IdentifierExpr : public Expression {
public:
    std::string name;
    
    void accept(ASTVisitor& visitor) override;
};

class BinaryExpr : public Expression {
public:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
    TokenType op;
    
    void accept(ASTVisitor& visitor) override;
};

class UnaryExpr : public Expression {
public:
    std::shared_ptr<Expression> operand;
    TokenType op;
    bool isPrefix;
    
    void accept(ASTVisitor& visitor) override;
};

class CallExpr : public Expression {
public:
    std::shared_ptr<Expression> callee;
    std::vector<std::shared_ptr<Expression>> args;
    
    void accept(ASTVisitor& visitor) override;
};

class CastExpr : public Expression {
public:
    std::shared_ptr<Expression> expr;
    std::shared_ptr<Type> targetType;
    
    void accept(ASTVisitor& visitor) override;
};

class IndexExpr : public Expression {
public:
    std::shared_ptr<Expression> base;
    std::shared_ptr<Expression> index;
    
    void accept(ASTVisitor& visitor) override;
};

class MemberAccessExpr : public Expression {
public:
    std::shared_ptr<Expression> object;
    std::string member;
    
    void accept(ASTVisitor& visitor) override;
};

// Inline Assembly
class AsmExpr : public Expression {
public:
    std::string asmString;
    bool isVolatile;
    std::vector<std::shared_ptr<Expression>> outputs;
    std::vector<std::shared_ptr<Expression>> inputs;
    std::vector<std::string> clobbers;
    
    void accept(ASTVisitor& visitor) override;
};

// Statements
class Statement : public ASTNode {
public:
};

class BlockStmt : public Statement {
public:
    std::vector<std::shared_ptr<Statement>> statements;
    
    void accept(ASTVisitor& visitor) override;
};

class LetStmt : public Statement {
public:
    std::string name;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expression> init;
    bool isMutable;
    
    void accept(ASTVisitor& visitor) override;
};

class ExprStmt : public Statement {
public:
    std::shared_ptr<Expression> expr;
    
    void accept(ASTVisitor& visitor) override;
};

class ReturnStmt : public Statement {
public:
    std::shared_ptr<Expression> expr;
    
    void accept(ASTVisitor& visitor) override;
};

class IfStmt : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> thenBranch;
    std::shared_ptr<Statement> elseBranch;
    
    void accept(ASTVisitor& visitor) override;
};

class WhileStmt : public Statement {
public:
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> body;
    
    void accept(ASTVisitor& visitor) override;
};

class ForStmt : public Statement {
public:
    std::shared_ptr<Statement> init;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Expression> update;
    std::shared_ptr<Statement> body;
    
    void accept(ASTVisitor& visitor) override;
};

// Declarations
class Declaration : public ASTNode {
public:
};

class FunctionDecl : public Declaration {
public:
    std::string name;
    std::shared_ptr<Type> returnType;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> params;
    std::shared_ptr<BlockStmt> body;
    bool isExtern;
    bool isVariadic;
    
    void accept(ASTVisitor& visitor) override;
};

class StructDecl : public Declaration {
public:
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> fields;
    
    void accept(ASTVisitor& visitor) override;
};

class EnumDecl : public Declaration {
public:
    std::string name;
    std::shared_ptr<Type> baseType;
    std::vector<std::pair<std::string, int64_t>> values;
    
    void accept(ASTVisitor& visitor) override;
};

// AST Visitor
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visitLiteral(LiteralExpr* expr) = 0;
    virtual void visitIdentifier(IdentifierExpr* expr) = 0;
    virtual void visitBinary(BinaryExpr* expr) = 0;
    virtual void visitUnary(UnaryExpr* expr) = 0;
    virtual void visitCall(CallExpr* expr) = 0;
    virtual void visitCast(CastExpr* expr) = 0;
    virtual void visitIndex(IndexExpr* expr) = 0;
    virtual void visitMemberAccess(MemberAccessExpr* expr) = 0;
    virtual void visitAsm(AsmExpr* expr) = 0;
    
    virtual void visitBlock(BlockStmt* stmt) = 0;
    virtual void visitLet(LetStmt* stmt) = 0;
    virtual void visitExpr(ExprStmt* stmt) = 0;
    virtual void visitReturn(ReturnStmt* stmt) = 0;
    virtual void visitIf(IfStmt* stmt) = 0;
    virtual void visitWhile(WhileStmt* stmt) = 0;
    virtual void visitFor(ForStmt* stmt) = 0;
    
    virtual void visitFunction(FunctionDecl* decl) = 0;
    virtual void visitStruct(StructDecl* decl) = 0;
    virtual void visitEnum(EnumDecl* decl) = 0;
};

// Program
class Program {
public:
    std::vector<std::shared_ptr<Declaration>> declarations;
};

} // namespace syclang

#endif // SYCLANG_PARSER_AST_H
