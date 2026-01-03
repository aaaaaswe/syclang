#ifndef SYCLANG_PARSER_PARSER_H
#define SYCLANG_PARSER_PARSER_H

#include <memory>
#include <vector>
#include "lexer/lexer.h"
#include "ast.h"

namespace syclang {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    
    // Parse entire program
    std::shared_ptr<Program> parse();
    
    // Get error messages
    const std::vector<std::string>& getErrors() const { return errors_; }
    
private:
    const std::vector<Token>& tokens_;
    size_t position_;
    std::vector<std::string> errors_;
    
    // Token operations
    Token current() const;
    Token peek(size_t offset = 0) const;
    bool advance();
    bool match(TokenType type);
    bool consume(TokenType type, const std::string& message);
    
    // Error reporting
    void error(const std::string& message);
    
    // Parsing declarations
    std::shared_ptr<Declaration> parseDeclaration();
    std::shared_ptr<FunctionDecl> parseFunctionDecl();
    std::shared_ptr<StructDecl> parseStructDecl();
    std::shared_ptr<EnumDecl> parseEnumDecl();
    
    // Parsing statements
    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<BlockStmt> parseBlock();
    std::shared_ptr<LetStmt> parseLet();
    std::shared_ptr<ReturnStmt> parseReturn();
    std::shared_ptr<IfStmt> parseIf();
    std::shared_ptr<WhileStmt> parseWhile();
    std::shared_ptr<ForStmt> parseFor();
    std::shared_ptr<ExprStmt> parseExprStmt();
    
    // Parsing expressions
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<Expression> parseAssignment();
    std::shared_ptr<Expression> parseLogicalOr();
    std::shared_ptr<Expression> parseLogicalAnd();
    std::shared_ptr<Expression> parseBitwiseOr();
    std::shared_ptr<Expression> parseBitwiseXor();
    std::shared_ptr<Expression> parseBitwiseAnd();
    std::shared_ptr<Expression> parseEquality();
    std::shared_ptr<Expression> parseComparison();
    std::shared_ptr<Expression> parseShift();
    std::shared_ptr<Expression> parseAdditive();
    std::shared_ptr<Expression> parseMultiplicative();
    std::shared_ptr<Expression> parsePrefix();
    std::shared_ptr<Expression> parsePostfix();
    std::shared_ptr<Expression> parsePrimary();
    
    // Type parsing
    std::shared_ptr<Type> parseType();
    
    // Utility
    bool isType(TokenType type);
};

} // namespace syclang

#endif // SYCLANG_PARSER_PARSER_H
