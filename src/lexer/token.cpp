#include "syclang/lexer/token.h"

namespace syclang {

bool Token::isChineseKeyword() const {
    // 检查值中是否包含中文字符
    for (char c : value_) {
        if (c >= 0x4E00 && c <= 0x9FFF) { // 中文字符范围
            return true;
        }
    }
    return false;
}

std::string Token::toString() const {
    std::string typeName;
    switch (type_) {
        case TokenType::KW_FN: typeName = "FN"; break;
        case TokenType::KW_LET: typeName = "LET"; break;
        case TokenType::KW_MUT: typeName = "MUT"; break;
        case TokenType::KW_CONST: typeName = "CONST"; break;
        case TokenType::KW_IF: typeName = "IF"; break;
        case TokenType::KW_ELSE: typeName = "ELSE"; break;
        case TokenType::KW_WHILE: typeName = "WHILE"; break;
        case TokenType::KW_FOR: typeName = "FOR"; break;
        case TokenType::KW_RETURN: typeName = "RETURN"; break;
        case TokenType::KW_STRUCT: typeName = "STRUCT"; break;
        case TokenType::KW_ENUM: typeName = "ENUM"; break;
        case TokenType::KW_UNION: typeName = "UNION"; break;
        case TokenType::KW_TRAIT: typeName = "TRAIT"; break;
        case TokenType::KW_IMPL: typeName = "IMPL"; break;
        case TokenType::KW_TRUE: typeName = "TRUE"; break;
        case TokenType::KW_FALSE: typeName = "FALSE"; break;
        case TokenType::KW_NULL: typeName = "NULL"; break;
        case TokenType::KW_ASM: typeName = "ASM"; break;
        case TokenType::KW_VOLATILE: typeName = "VOLATILE"; break;
        case TokenType::KW_ASYNC: typeName = "ASYNC"; break;
        case TokenType::KW_AWAIT: typeName = "AWAIT"; break;
        case TokenType::KW_MATCH: typeName = "MATCH"; break;
        case TokenType::TYPE_I8: typeName = "I8"; break;
        case TokenType::TYPE_I16: typeName = "I16"; break;
        case TokenType::TYPE_I32: typeName = "I32"; break;
        case TokenType::TYPE_I64: typeName = "I64"; break;
        case TokenType::TYPE_U8: typeName = "U8"; break;
        case TokenType::TYPE_U16: typeName = "U16"; break;
        case TokenType::TYPE_U32: typeName = "U32"; break;
        case TokenType::TYPE_U64: typeName = "U64"; break;
        case TokenType::TYPE_F32: typeName = "F32"; break;
        case TokenType::TYPE_F64: typeName = "F64"; break;
        case TokenType::TYPE_BOOL: typeName = "BOOL"; break;
        case TokenType::TYPE_VOID: typeName = "VOID"; break;
        case TokenType::TYPE_CHAR: typeName = "CHAR"; break;
        case TokenType::TYPE_STRING: typeName = "STRING"; break;
        case TokenType::TYPE_UNIQUE_PTR: typeName = "UNIQUE_PTR"; break;
        case TokenType::TYPE_SHARED_PTR: typeName = "SHARED_PTR"; break;
        case TokenType::TYPE_WEAK_PTR: typeName = "WEAK_PTR"; break;
        case TokenType::TYPE_VECTOR: typeName = "VECTOR"; break;
        case TokenType::TYPE_MAP: typeName = "MAP"; break;
        case TokenType::TYPE_SET: typeName = "SET"; break;
        case TokenType::PLUS: typeName = "PLUS"; break;
        case TokenType::MINUS: typeName = "MINUS"; break;
        case TokenType::STAR: typeName = "STAR"; break;
        case TokenType::SLASH: typeName = "SLASH"; break;
        case TokenType::PERCENT: typeName = "PERCENT"; break;
        case TokenType::PLUS_PLUS: typeName = "PLUS_PLUS"; break;
        case TokenType::MINUS_MINUS: typeName = "MINUS_MINUS"; break;
        case TokenType::EQUAL: typeName = "EQUAL"; break;
        case TokenType::PLUS_EQUAL: typeName = "PLUS_EQUAL"; break;
        case TokenType::MINUS_EQUAL: typeName = "MINUS_EQUAL"; break;
        case TokenType::STAR_EQUAL: typeName = "STAR_EQUAL"; break;
        case TokenType::SLASH_EQUAL: typeName = "SLASH_EQUAL"; break;
        case TokenType::PERCENT_EQUAL: typeName = "PERCENT_EQUAL"; break;
        case TokenType::LESS: typeName = "LESS"; break;
        case TokenType::GREATER: typeName = "GREATER"; break;
        case TokenType::LESS_EQUAL: typeName = "LESS_EQUAL"; break;
        case TokenType::GREATER_EQUAL: typeName = "GREATER_EQUAL"; break;
        case TokenType::EQUAL_EQUAL: typeName = "EQUAL_EQUAL"; break;
        case TokenType::NOT_EQUAL: typeName = "NOT_EQUAL"; break;
        case TokenType::AND: typeName = "AND"; break;
        case TokenType::OR: typeName = "OR"; break;
        case TokenType::NOT: typeName = "NOT"; break;
        case TokenType::AND_AND: typeName = "AND_AND"; break;
        case TokenType::OR_OR: typeName = "OR_OR"; break;
        case TokenType::BIT_AND: typeName = "BIT_AND"; break;
        case TokenType::BIT_OR: typeName = "BIT_OR"; break;
        case TokenType::BIT_XOR: typeName = "BIT_XOR"; break;
        case TokenType::BIT_NOT: typeName = "BIT_NOT"; break;
        case TokenType::SHL: typeName = "SHL"; break;
        case TokenType::SHR: typeName = "SHR"; break;
        case TokenType::PIPE: typeName = "PIPE"; break;
        case TokenType::RANGE: typeName = "RANGE"; break;
        case TokenType::SPREAD: typeName = "SPREAD"; break;
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
        case TokenType::ARROW: typeName = "ARROW"; break;
        case TokenType::FAT_ARROW: typeName = "FAT_ARROW"; break;
        case TokenType::QUESTION: typeName = "QUESTION"; break;
        case TokenType::AT_SIGN: typeName = "AT_SIGN"; break;
        case TokenType::CHINESE_COMMENT_START: typeName = "CHINESE_COMMENT_START"; break;
        case TokenType::CHINESE_COMMENT_END: typeName = "CHINESE_COMMENT_END"; break;
        case TokenType::EOF_TOKEN: typeName = "EOF"; break;
        default: typeName = "UNKNOWN"; break;
    }
    
    std::string result = typeName + "(" + value_ + ")";
    if (!chineseDescription_.empty()) {
        result += " [" + chineseDescription_ + "]";
    }
    return result;
}

} // namespace syclang
