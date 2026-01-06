#include "syclang/lexer/lexer.h"
#include <cctype>
#include <iostream>

namespace syclang {

Lexer::Lexer(const std::string& source)
    : source_(source), position_(0), line_(1), column_(1) {}

char Lexer::peek(size_t offset) {
    if (position_ + offset >= source_.size()) {
        return '\0';
    }
    return source_[position_ + offset];
}

char Lexer::advance() {
    char c = peek();
    if (c != '\0') {
        position_++;
        if (c == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        // Single line comment
        while (peek() != '\0' && peek() != '\n') {
            advance();
        }
    } else if (peek() == '/' && peek(1) == '*') {
        // Multi-line comment
        advance(); // /
        advance(); // *
        while (!(peek() == '*' && peek(1) == '/') && peek() != '\0') {
            advance();
        }
        if (peek() == '*' && peek(1) == '/') {
            advance(); // *
            advance(); // /
        }
    }
}

Token Lexer::scanNumber() {
    size_t start = position_;
    bool isFloat = false;
    
    while (isdigit(peek()) || peek() == '.') {
        if (peek() == '.') {
            isFloat = true;
        }
        advance();
    }
    
    std::string value = source_.substr(start, position_ - start);
    return Token(TokenType::NUMBER, value, line_, column_);
}

Token Lexer::scanString() {
    advance(); // Skip opening quote
    size_t start = position_;
    
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            advance(); // Skip escape character
        }
        advance();
    }
    
    std::string value = source_.substr(start, position_ - start);
    if (peek() == '"') {
        advance(); // Skip closing quote
    }
    
    return Token(TokenType::STRING, value, line_, column_);
}

Token Lexer::scanIdentifier() {
    size_t start = position_;
    
    while (isalnum(peek()) || peek() == '_') {
        advance();
    }
    
    std::string value = source_.substr(start, position_ - start);
    
    // Check for keywords
    if (value == "fn") return Token(TokenType::KW_FN, value, line_, column_);
    if (value == "let") return Token(TokenType::KW_LET, value, line_, column_);
    if (value == "mut") return Token(TokenType::KW_MUT, value, line_, column_);
    if (value == "if") return Token(TokenType::KW_IF, value, line_, column_);
    if (value == "else") return Token(TokenType::KW_ELSE, value, line_, column_);
    if (value == "while") return Token(TokenType::KW_WHILE, value, line_, column_);
    if (value == "for") return Token(TokenType::KW_FOR, value, line_, column_);
    if (value == "return") return Token(TokenType::KW_RETURN, value, line_, column_);
    if (value == "struct") return Token(TokenType::KW_STRUCT, value, line_, column_);
    if (value == "enum") return Token(TokenType::KW_ENUM, value, line_, column_);
    if (value == "union") return Token(TokenType::KW_UNION, value, line_, column_);
    if (value == "extern") return Token(TokenType::KW_EXTERN, value, line_, column_);
    if (value == "true") return Token(TokenType::KW_TRUE, value, line_, column_);
    if (value == "false") return Token(TokenType::KW_FALSE, value, line_, column_);
    if (value == "null") return Token(TokenType::KW_NULL, value, line_, column_);
    if (value == "asm") return Token(TokenType::KW_ASM, value, line_, column_);
    if (value == "volatile") return Token(TokenType::KW_VOLATILE, value, line_, column_);
    if (value == "i8") return Token(TokenType::TYPE_I8, value, line_, column_);
    if (value == "i16") return Token(TokenType::TYPE_I16, value, line_, column_);
    if (value == "i32") return Token(TokenType::TYPE_I32, value, line_, column_);
    if (value == "i64") return Token(TokenType::TYPE_I64, value, line_, column_);
    if (value == "u8") return Token(TokenType::TYPE_U8, value, line_, column_);
    if (value == "u16") return Token(TokenType::TYPE_U16, value, line_, column_);
    if (value == "u32") return Token(TokenType::TYPE_U32, value, line_, column_);
    if (value == "u64") return Token(TokenType::TYPE_U64, value, line_, column_);
    if (value == "f32") return Token(TokenType::TYPE_F32, value, line_, column_);
    if (value == "f64") return Token(TokenType::TYPE_F64, value, line_, column_);
    if (value == "bool") return Token(TokenType::TYPE_BOOL, value, line_, column_);
    if (value == "void") return Token(TokenType::TYPE_VOID, value, line_, column_);
    
    return Token(TokenType::IDENTIFIER, value, line_, column_);
}

Token Lexer::scanSymbol() {
    char c = advance();
    char next = peek();
    
    switch (c) {
        case '(': return Token(TokenType::LPAREN, "(", line_, column_);
        case ')': return Token(TokenType::RPAREN, ")", line_, column_);
        case '{': return Token(TokenType::LBRACE, "{", line_, column_);
        case '}': return Token(TokenType::RBRACE, "}", line_, column_);
        case '[': return Token(TokenType::LBRACKET, "[", line_, column_);
        case ']': return Token(TokenType::RBRACKET, "]", line_, column_);
        case ';': return Token(TokenType::SEMICOLON, ";", line_, column_);
        case ':': 
            if (next == ':') { advance(); return Token(TokenType::COLON, "::", line_, column_); }
            return Token(TokenType::COLON, ":", line_, column_);
        case ',': return Token(TokenType::COMMA, ",", line_, column_);
        case '.': return Token(TokenType::DOT, ".", line_, column_);
        
        case '+':
            if (next == '+') { advance(); return Token(TokenType::PLUS_PLUS, "++", line_, column_); }
            if (next == '=') { advance(); return Token(TokenType::PLUS_EQUAL, "+=", line_, column_); }
            return Token(TokenType::PLUS, "+", line_, column_);
            
        case '-':
            if (next == '-') { advance(); return Token(TokenType::MINUS_MINUS, "--", line_, column_); }
            if (next == '=') { advance(); return Token(TokenType::MINUS_EQUAL, "-=", line_, column_); }
            if (next == '>') { advance(); return Token(TokenType::ARROW, "->", line_, column_); }
            return Token(TokenType::MINUS, "-", line_, column_);
            
        case '*':
            if (next == '=') { advance(); return Token(TokenType::STAR_EQUAL, "*=", line_, column_); }
            return Token(TokenType::STAR, "*", line_, column_);
            
        case '/':
            if (next == '=') { advance(); return Token(TokenType::SLASH_EQUAL, "/=", line_, column_); }
            return Token(TokenType::SLASH, "/", line_, column_);
            
        case '%':
            if (next == '=') { advance(); return Token(TokenType::PERCENT_EQUAL, "%=", line_, column_); }
            return Token(TokenType::PERCENT, "%", line_, column_);
            
        case '=':
            if (next == '=') { advance(); return Token(TokenType::EQUAL_EQUAL, "==", line_, column_); }
            if (next == '>') { advance(); return Token(TokenType::FAT_ARROW, "=>", line_, column_); }
            return Token(TokenType::EQUAL, "=", line_, column_);
            
        case '!':
            if (next == '=') { advance(); return Token(TokenType::NOT_EQUAL, "!=", line_, column_); }
            return Token(TokenType::NOT, "!", line_, column_);
            
        case '<':
            if (next == '=') { advance(); return Token(TokenType::LESS_EQUAL, "<=", line_, column_); }
            if (next == '<') { advance(); return Token(TokenType::SHL, "<<", line_, column_); }
            return Token(TokenType::LESS, "<", line_, column_);
            
        case '>':
            if (next == '=') { advance(); return Token(TokenType::GREATER_EQUAL, ">=", line_, column_); }
            if (next == '>') { advance(); return Token(TokenType::SHR, ">>", line_, column_); }
            return Token(TokenType::GREATER, ">", line_, column_);
            
        case '&':
            if (next == '&') { advance(); return Token(TokenType::AND_AND, "&&", line_, column_); }
            return Token(TokenType::BIT_AND, "&", line_, column_);
            
        case '|':
            if (next == '|') { advance(); return Token(TokenType::OR_OR, "||", line_, column_); }
            return Token(TokenType::BIT_OR, "|", line_, column_);
            
        case '^':
            return Token(TokenType::BIT_XOR, "^", line_, column_);
            
        case '~':
            return Token(TokenType::BIT_NOT, "~", line_, column_);
            
        case '?':
            return Token(TokenType::QUESTION, "?", line_, column_);
            
        default:
            return Token(TokenType::UNKNOWN, std::string(1, c), line_, column_);
    }
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    // Check for comments
    if (peek() == '/' && (peek(1) == '/' || peek(1) == '*')) {
        skipComment();
        return nextToken(); // Recurse to get next token
    }
    
    if (position_ >= source_.size()) {
        return Token(TokenType::EOF_TOKEN, "", line_, column_);
    }
    
    char c = peek();
    
    if (isdigit(c)) {
        return scanNumber();
    }
    
    if (c == '"') {
        return scanString();
    }
    
    if (isalpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    return scanSymbol();
}

Token Lexer::peekToken(size_t offset) {
    size_t savedPos = position_;
    size_t savedLine = line_;
    size_t savedCol = column_;
    
    Token token = nextToken();
    for (size_t i = 0; i < offset && !token.is(TokenType::EOF_TOKEN); ++i) {
        token = nextToken();
    }
    
    position_ = savedPos;
    line_ = savedLine;
    column_ = savedCol;
    
    return token;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = nextToken();
    
    while (!token.is(TokenType::EOF_TOKEN)) {
        tokens.push_back(token);
        token = nextToken();
    }
    
    tokens.push_back(token); // Add EOF token
    return tokens;
}

} // namespace syclang
