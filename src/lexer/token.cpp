#include "syclang/lexer/token.h"

namespace syclang {

std::string Token::toString() const {
    std::string typeName;
    switch (type_) {
        case TokenType::KW_FN: typeName = "FN"; break;
        case TokenType::KW_LET: typeName = "LET"; break;
        case TokenType::KW_MUT: typeName = "MUT"; break;
        case TokenType::KW_IF: typeName = "IF"; break;
        case TokenType::KW_ELSE: typeName = "ELSE"; break;
        case TokenType::KW_WHILE: typeName = "WHILE"; break;
        case TokenType::KW_FOR: typeName = "FOR"; break;
        case TokenType::KW_RETURN: typeName = "RETURN"; break;
        case TokenType::KW_STRUCT: typeName = "STRUCT"; break;
        case TokenType::KW_ENUM: typeName = "ENUM"; break;
        case TokenType::KW_EXTERN: typeName = "EXTERN"; break;
        case TokenType::KW_TRUE: typeName = "TRUE"; break;
        case TokenType::KW_FALSE: typeName = "FALSE"; break;
        case TokenType::KW_NULL: typeName = "NULL"; break;
        case TokenType::KW_ASM: typeName = "ASM"; break;
        case TokenType::KW_VOLATILE: typeName = "VOLATILE"; break;
        case TokenType::PLUS: typeName = "PLUS"; break;
        case TokenType::MINUS: typeName = "MINUS"; break;
        case TokenType::STAR: typeName = "STAR"; break;
        case TokenType::SLASH: typeName = "SLASH"; break;
        case TokenType::PERCENT: typeName = "PERCENT"; break;
        case TokenType::LPAREN: typeName = "LPAREN"; break;
        case TokenType::RPAREN: typeName = "RPAREN"; break;
        case TokenType::LBRACE: typeName = "LBRACE"; break;
        case TokenType::RBRACE: typeName = "RBRACE"; break;
        case TokenType::LBRACKET: typeName = "LBRACKET"; break;
        case TokenType::RBRACKET: typeName = "RBRACKET"; break;
        case TokenType::SEMICOLON: typeName = "SEMICOLON"; break;
        case TokenType::COLON: typeName = "COLON"; break;
        case TokenType::COMMA: typeName = "COMMA"; break;
        case TokenType::DOT: typeName = "DOT"; break;
        case TokenType::EOF_TOKEN: typeName = "EOF"; break;
        default: typeName = "UNKNOWN"; break;
    }
    return typeName + "(" + value_ + ")";
}

} // namespace syclang
