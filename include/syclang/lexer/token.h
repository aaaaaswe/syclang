#ifndef SYCLANG_LEXER_TOKEN_H
#define SYCLANG_LEXER_TOKEN_H

#include <string>

namespace syclang {

enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_MUT, KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_RETURN, KW_STRUCT, KW_ENUM, KW_UNION, KW_EXTERN,
    KW_TRUE, KW_FALSE, KW_NULL,
    KW_ASM, KW_VOLATILE, KW_ALIGN, KW_PACKED,
    
    // Types
    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64,
    TYPE_F32, TYPE_F64,
    TYPE_BOOL, TYPE_VOID,
    TYPE_PTR, TYPE_ARRAY,
    
    // Identifiers and Literals
    IDENTIFIER,
    NUMBER,
    STRING,
    CHAR,
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    PLUS_PLUS, MINUS_MINUS,
    EQUAL, PLUS_EQUAL, MINUS_EQUAL,
    STAR_EQUAL, SLASH_EQUAL, PERCENT_EQUAL,
    
    // Comparison
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    EQUAL_EQUAL, NOT_EQUAL,
    
    // Logical
    AND, OR, NOT, AND_AND, OR_OR,
    
    // Bitwise
    BIT_AND, BIT_OR, BIT_XOR, BIT_NOT,
    SHL, SHR,
    
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE,
    LBRACKET, RBRACKET,
    SEMICOLON, COLON, COMMA, DOT,
    ARROW, FAT_ARROW, QUESTION,
    
    // Special
    EOF_TOKEN,
    UNKNOWN
};

class Token {
public:
    Token() : type_(TokenType::UNKNOWN), line_(0), column_(0) {}
    
    Token(TokenType type, std::string value, size_t line, size_t column)
        : type_(type), value_(std::move(value)), line_(line), column_(column) {}
    
    TokenType type() const { return type_; }
    const std::string& value() const { return value_; }
    size_t line() const { return line_; }
    size_t column() const { return column_; }
    
    bool is(TokenType type) const { return type_ == type; }
    
    std::string toString() const;

private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

} // namespace syclang

#endif // SYCLANG_LEXER_TOKEN_H
