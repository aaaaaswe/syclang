#include "syclang/parser/parser.h"
#include <iostream>

namespace syclang {

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), position_(0) {}

Token Parser::current() const {
    if (position_ < tokens_.size()) {
        return tokens_[position_];
    }
    return Token(TokenType::EOF_TOKEN, "", 0, 0);
}

Token Parser::peek(size_t offset) const {
    if (position_ + offset < tokens_.size()) {
        return tokens_[position_ + offset];
    }
    return Token(TokenType::EOF_TOKEN, "", 0, 0);
}

bool Parser::advance() {
    if (!current().is(TokenType::EOF_TOKEN)) {
        position_++;
        return true;
    }
    return false;
}

bool Parser::match(TokenType type) {
    if (current().is(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::consume(TokenType type, const std::string& message) {
    if (match(type)) {
        return true;
    }
    error(message);
    return false;
}

void Parser::error(const std::string& message) {
    Token token = current();
    std::string error_msg = "Error at line " + std::to_string(token.line()) + 
                           ", column " + std::to_string(token.column()) + 
                           ": " + message;
    errors_.push_back(error_msg);
}

std::shared_ptr<Program> Parser::parse() {
    auto program = std::make_shared<Program>();
    
    while (!current().is(TokenType::EOF_TOKEN)) {
        auto decl = parseDeclaration();
        if (decl) {
            program->declarations.push_back(decl);
        }
    }
    
    return program;
}

std::shared_ptr<Declaration> Parser::parseDeclaration() {
    Token token = current();
    
    if (match(TokenType::KW_FN)) {
        return parseFunctionDecl();
    }
    
    if (match(TokenType::KW_STRUCT)) {
        return parseStructDecl();
    }
    
    if (match(TokenType::KW_ENUM)) {
        return parseEnumDecl();
    }
    
    error("Expected function, struct, or enum declaration");
    advance(); // Skip the problematic token to avoid infinite loop
    return nullptr;
}

std::shared_ptr<FunctionDecl> Parser::parseFunctionDecl() {
    auto func = std::make_shared<FunctionDecl>();
    
    // Function name
    Token nameToken = current();
    if (!consume(TokenType::IDENTIFIER, "Expected function name")) {
        return nullptr;
    }
    func->name = nameToken.value();
    
    // Parameters
    if (!consume(TokenType::LPAREN, "Expected '('")) {
        return nullptr;
    }
    
    while (!match(TokenType::RPAREN)) {
        Token paramName = current();
        if (!consume(TokenType::IDENTIFIER, "Expected parameter name")) {
            // Skip to next comma or closing parenthesis
            while (!current().is(TokenType::COMMA) &&
                   !current().is(TokenType::RPAREN) &&
                   !current().is(TokenType::EOF_TOKEN)) {
                advance();
            }
            if (match(TokenType::COMMA)) {
                continue;
            }
            break;
        }
        if (!consume(TokenType::COLON, "Expected ':'")) {
            // Skip to next comma or closing parenthesis
            while (!current().is(TokenType::COMMA) &&
                   !current().is(TokenType::RPAREN) &&
                   !current().is(TokenType::EOF_TOKEN)) {
                advance();
            }
            if (match(TokenType::COMMA)) {
                continue;
            }
            break;
        }
        auto paramType = parseType();
        
        func->params.push_back({paramName.value(), paramType});
        
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    // Return type
    if (!consume(TokenType::ARROW, "Expected '->'")) {
        // Try to parse anyway, but skip return type parsing
        while (!current().is(TokenType::LBRACE) &&
               !current().is(TokenType::KW_EXTERN) &&
               !current().is(TokenType::EOF_TOKEN)) {
            advance();
        }
    } else {
        func->returnType = parseType();
    }
    
    // Function body or extern
    if (match(TokenType::KW_EXTERN)) {
        func->isExtern = true;
    } else {
        func->body = parseBlock();
    }
    
    return func;
}

std::shared_ptr<StructDecl> Parser::parseStructDecl() {
    auto structDecl = std::make_shared<StructDecl>();
    
    Token nameToken = current();
    consume(TokenType::IDENTIFIER, "Expected struct name");
    structDecl->name = nameToken.value();
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!match(TokenType::RBRACE)) {
        Token fieldName = current();
        consume(TokenType::IDENTIFIER, "Expected field name");
        consume(TokenType::COLON, "Expected ':'");
        auto fieldType = parseType();
        
        structDecl->fields.push_back({fieldName.value(), fieldType});
        consume(TokenType::SEMICOLON, "Expected ';'");
    }
    
    return structDecl;
}

std::shared_ptr<EnumDecl> Parser::parseEnumDecl() {
    auto enumDecl = std::make_shared<EnumDecl>();
    
    Token nameToken = current();
    consume(TokenType::IDENTIFIER, "Expected enum name");
    enumDecl->name = nameToken.value();
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    int64_t value = 0;
    while (!match(TokenType::RBRACE)) {
        Token variantName = current();
        consume(TokenType::IDENTIFIER, "Expected variant name");
        
        if (match(TokenType::EQUAL)) {
            Token valueToken = current();
            consume(TokenType::NUMBER, "Expected enum value");
            value = std::stoll(valueToken.value());
        }
        
        enumDecl->values.push_back({variantName.value(), value++});
        
        if (!match(TokenType::COMMA)) {
            break;
        }
    }
    
    return enumDecl;
}

std::shared_ptr<Statement> Parser::parseStatement() {
    Token token = current();
    
    if (match(TokenType::KW_LET)) {
        return parseLet();
    }
    
    if (match(TokenType::KW_IF)) {
        return parseIf();
    }
    
    if (match(TokenType::KW_WHILE)) {
        return parseWhile();
    }
    
    if (match(TokenType::KW_FOR)) {
        return parseFor();
    }
    
    if (match(TokenType::KW_RETURN)) {
        return parseReturn();
    }
    
    if (current().is(TokenType::LBRACE)) {
        return parseBlock();
    }
    
    // If we reach here, this might be an expression statement
    // Try to parse it as an expression, but handle errors gracefully
    auto exprStmt = parseExprStmt();
    if (!exprStmt->expr) {
        // If parsing failed, skip to the next semicolon or other delimiter
        while (!current().is(TokenType::SEMICOLON) &&
               !current().is(TokenType::RBRACE) &&
               !current().is(TokenType::EOF_TOKEN)) {
            advance();
        }
        if (match(TokenType::SEMICOLON)) {
            // Consumed semicolon
        }
    }
    return exprStmt;
}

std::shared_ptr<BlockStmt> Parser::parseBlock() {
    auto block = std::make_shared<BlockStmt>();
    
    if (!consume(TokenType::LBRACE, "Expected '{'")) {
        // Skip to the next closing brace to avoid infinite loop
        while (!current().is(TokenType::RBRACE) &&
               !current().is(TokenType::EOF_TOKEN)) {
            advance();
        }
        if (match(TokenType::RBRACE)) {
            // Consumed closing brace
        }
        return nullptr;
    }
    
    while (!current().is(TokenType::RBRACE) && !current().is(TokenType::EOF_TOKEN)) {
        auto stmt = parseStatement();
        if (stmt) {
            block->statements.push_back(stmt);
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return block;
}

std::shared_ptr<LetStmt> Parser::parseLet() {
    auto let = std::make_shared<LetStmt>();
    
    let->isMutable = match(TokenType::KW_MUT);
    
    Token nameToken = current();
    if (!consume(TokenType::IDENTIFIER, "Expected variable name")) {
        // Skip to next semicolon
        while (!current().is(TokenType::SEMICOLON) &&
               !current().is(TokenType::RBRACE) &&
               !current().is(TokenType::EOF_TOKEN)) {
            advance();
        }
        consume(TokenType::SEMICOLON, "Expected ';'");
        return nullptr;
    }
    let->name = nameToken.value();
    
    if (match(TokenType::COLON)) {
        let->type = parseType();
    }
    
    if (match(TokenType::EQUAL)) {
        let->init = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return let;
}

std::shared_ptr<ExprStmt> Parser::parseExprStmt() {
    auto stmt = std::make_shared<ExprStmt>();
    stmt->expr = parseExpression();
    if (stmt->expr) {
        consume(TokenType::SEMICOLON, "Expected ';'");
    } else {
        // Skip to next semicolon or other statement delimiter
        while (!current().is(TokenType::SEMICOLON) &&
               !current().is(TokenType::RBRACE) &&
               !current().is(TokenType::EOF_TOKEN)) {
            advance();
        }
        if (match(TokenType::SEMICOLON)) {
            // Consumed semicolon
        }
    }
    return stmt;
}

std::shared_ptr<ReturnStmt> Parser::parseReturn() {
    auto ret = std::make_shared<ReturnStmt>();
    
    if (!current().is(TokenType::SEMICOLON)) {
        ret->expr = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return ret;
}

std::shared_ptr<IfStmt> Parser::parseIf() {
    auto ifStmt = std::make_shared<IfStmt>();
    
    consume(TokenType::LPAREN, "Expected '('");
    ifStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    ifStmt->thenBranch = parseStatement();
    
    if (match(TokenType::KW_ELSE)) {
        ifStmt->elseBranch = parseStatement();
    }
    
    return ifStmt;
}

std::shared_ptr<WhileStmt> Parser::parseWhile() {
    auto whileStmt = std::make_shared<WhileStmt>();
    
    consume(TokenType::LPAREN, "Expected '('");
    whileStmt->condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')'");
    
    whileStmt->body = parseStatement();
    
    return whileStmt;
}

std::shared_ptr<ForStmt> Parser::parseFor() {
    auto forStmt = std::make_shared<ForStmt>();
    
    consume(TokenType::LPAREN, "Expected '('");
    
    if (!match(TokenType::SEMICOLON)) {
        forStmt->init = parseStatement();
    }
    
    if (!match(TokenType::SEMICOLON)) {
        forStmt->condition = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';'");
    }
    
    if (!current().is(TokenType::RPAREN)) {
        forStmt->update = parseExpression();
    }
    
    consume(TokenType::RPAREN, "Expected ')'");
    forStmt->body = parseStatement();
    
    return forStmt;
}

std::shared_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<Expression> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    
    if (current().is(TokenType::EQUAL) ||
        current().is(TokenType::PLUS_EQUAL) ||
        current().is(TokenType::MINUS_EQUAL) ||
        current().is(TokenType::STAR_EQUAL) ||
        current().is(TokenType::SLASH_EQUAL) ||
        current().is(TokenType::PERCENT_EQUAL)) {
        
        Token op = current();
        advance();
        auto right = parseAssignment();
        
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = op.type();
        return binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TokenType::OR_OR)) {
        auto right = parseLogicalAnd();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = TokenType::OR_OR;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseLogicalAnd() {
    auto expr = parseBitwiseOr();
    
    while (match(TokenType::AND_AND)) {
        auto right = parseBitwiseOr();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = TokenType::AND_AND;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseBitwiseOr() {
    auto expr = parseBitwiseXor();
    
    while (match(TokenType::BIT_OR)) {
        auto right = parseBitwiseXor();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = TokenType::BIT_OR;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseBitwiseXor() {
    auto expr = parseBitwiseAnd();
    
    while (match(TokenType::BIT_XOR)) {
        auto right = parseBitwiseAnd();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = TokenType::BIT_XOR;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseBitwiseAnd() {
    auto expr = parseEquality();
    
    while (match(TokenType::BIT_AND)) {
        auto right = parseEquality();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = TokenType::BIT_AND;
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseEquality() {
    auto expr = parseComparison();
    
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL)) {
        auto right = parseComparison();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = tokens_[position_ - 1].type();
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseComparison() {
    auto expr = parseShift();
    
    while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
        auto right = parseShift();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = tokens_[position_ - 1].type();
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseShift() {
    auto expr = parseAdditive();
    
    while (match(TokenType::SHL) || match(TokenType::SHR)) {
        auto right = parseAdditive();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = tokens_[position_ - 1].type();
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        auto right = parseMultiplicative();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = tokens_[position_ - 1].type();
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parseMultiplicative() {
    auto expr = parsePrefix();
    
    while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
        auto right = parsePrefix();
        auto binary = std::make_shared<BinaryExpr>();
        binary->left = expr;
        binary->right = right;
        binary->op = tokens_[position_ - 1].type();
        expr = binary;
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parsePrefix() {
    if (match(TokenType::MINUS) || match(TokenType::NOT) || match(TokenType::BIT_NOT)) {
        auto unary = std::make_shared<UnaryExpr>();
        unary->op = tokens_[position_ - 1].type();
        unary->isPrefix = true;
        unary->operand = parsePrefix();
        return unary;
    }
    
    return parsePostfix();
}

std::shared_ptr<Expression> Parser::parsePostfix() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::LPAREN)) {
            // Function call
            auto call = std::make_shared<CallExpr>();
            call->callee = expr;
            
            while (!match(TokenType::RPAREN)) {
                call->args.push_back(parseExpression());
                if (!match(TokenType::COMMA)) {
                    break;
                }
            }
            
            expr = call;
        } else if (match(TokenType::LBRACKET)) {
            // Array index
            auto index = std::make_shared<IndexExpr>();
            index->base = expr;
            index->index = parseExpression();
            consume(TokenType::RBRACKET, "Expected ']'");
            expr = index;
        } else if (match(TokenType::DOT)) {
            // Member access
            auto access = std::make_shared<MemberAccessExpr>();
            access->object = expr;
            Token member = current();
            consume(TokenType::IDENTIFIER, "Expected member name");
            access->member = member.value();
            expr = access;
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto lit = std::make_shared<LiteralExpr>();
        lit->kind = LiteralExpr::Kind::INT;
        lit->value = tokens_[position_ - 1].value();
        return lit;
    }
    
    if (match(TokenType::STRING)) {
        auto lit = std::make_shared<LiteralExpr>();
        lit->kind = LiteralExpr::Kind::STRING;
        lit->value = tokens_[position_ - 1].value();
        return lit;
    }
    
    if (match(TokenType::KW_TRUE)) {
        auto lit = std::make_shared<LiteralExpr>();
        lit->kind = LiteralExpr::Kind::BOOL;
        lit->value = "true";
        return lit;
    }
    
    if (match(TokenType::KW_FALSE)) {
        auto lit = std::make_shared<LiteralExpr>();
        lit->kind = LiteralExpr::Kind::BOOL;
        lit->value = "false";
        return lit;
    }
    
    if (match(TokenType::IDENTIFIER)) {
        auto ident = std::make_shared<IdentifierExpr>();
        ident->name = tokens_[position_ - 1].value();
        return ident;
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    
    error("Expected expression");
    advance(); // Skip the problematic token to avoid infinite loop
    return nullptr;
}

std::shared_ptr<Type> Parser::parseType() {
    auto type = std::make_shared<Type>();
    Token token = current();
    
    switch (token.type()) {
        case TokenType::TYPE_I8:
            advance();
            type->category = TypeCategory::I8;
            break;
        case TokenType::TYPE_I16:
            advance();
            type->category = TypeCategory::I16;
            break;
        case TokenType::TYPE_I32:
            advance();
            type->category = TypeCategory::I32;
            break;
        case TokenType::TYPE_I64:
            advance();
            type->category = TypeCategory::I64;
            break;
        case TokenType::TYPE_U8:
            advance();
            type->category = TypeCategory::U8;
            break;
        case TokenType::TYPE_U16:
            advance();
            type->category = TypeCategory::U16;
            break;
        case TokenType::TYPE_U32:
            advance();
            type->category = TypeCategory::U32;
            break;
        case TokenType::TYPE_U64:
            advance();
            type->category = TypeCategory::U64;
            break;
        case TokenType::TYPE_F32:
            advance();
            type->category = TypeCategory::F32;
            break;
        case TokenType::TYPE_F64:
            advance();
            type->category = TypeCategory::F64;
            break;
        case TokenType::TYPE_BOOL:
            advance();
            type->category = TypeCategory::BOOL;
            break;
        case TokenType::TYPE_VOID:
            advance();
            type->category = TypeCategory::VOID;
            break;
        case TokenType::IDENTIFIER:
            advance();
            type->name = token.value();
            type->category = TypeCategory::STRUCT; // Assume struct for now
            break;
        default:
            error("Expected type");
            advance(); // Skip the problematic token to avoid infinite loop
            return nullptr;
    }
    
    // Handle pointers
    while (match(TokenType::STAR)) {
        auto ptrType = std::make_shared<Type>();
        ptrType->category = TypeCategory::POINTER;
        ptrType->baseType = type;
        type = ptrType;
    }
    
    return type;
}

bool Parser::isType(TokenType type) {
    return type == TokenType::TYPE_I8 || type == TokenType::TYPE_I16 ||
           type == TokenType::TYPE_I32 || type == TokenType::TYPE_I64 ||
           type == TokenType::TYPE_U8 || type == TokenType::TYPE_U16 ||
           type == TokenType::TYPE_U32 || type == TokenType::TYPE_U64 ||
           type == TokenType::TYPE_F32 || type == TokenType::TYPE_F64 ||
           type == TokenType::TYPE_BOOL || type == TokenType::TYPE_VOID ||
           type == TokenType::IDENTIFIER;
}

} // namespace syclang
