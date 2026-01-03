#include "syclang/lexer/chinese_lexer.h"
#include <algorithm>

namespace syclang {

ChineseKeywordMap& ChineseKeywordMap::getInstance() {
    static ChineseKeywordMap instance;
    return instance;
}

ChineseKeywordMap::ChineseKeywordMap() {
    initializeMap();
}

void ChineseKeywordMap::initializeMap() {
    // 函数相关
    chineseToEnglish_["计算"] = TokenType::KW_FN;
    chineseToEnglish_["函数"] = TokenType::KW_FN;
    chineseToEnglish_["功能"] = TokenType::KW_FN;
    
    // 控制流
    chineseToEnglish_["如果"] = TokenType::KW_IF;
    chineseToEnglish_["若"] = TokenType::KW_IF;
    chineseToEnglish_["否则"] = TokenType::KW_ELSE;
    chineseToEnglish_["其它"] = TokenType::KW_ELSE;
    
    // 循环
    chineseToEnglish_["循环"] = TokenType::KW_WHILE;
    chineseToEnglish_["当"] = TokenType::KW_WHILE;
    chineseToEnglish_["遍历"] = TokenType::KW_FOR;
    chineseToEnglish_["针对"] = TokenType::KW_FOR;
    
    // 返回
    chineseToEnglish_["返回"] = TokenType::KW_RETURN;
    chineseToEnglish_["回传"] = TokenType::KW_RETURN;
    
    // 变量
    chineseToEnglish_["变量"] = TokenType::KW_LET;
    chineseToEnglish_["设"] = TokenType::KW_LET;
    chineseToEnglish_["可变"] = TokenType::KW_MUT;
    chineseToEnglish_["常量"] = TokenType::KW_CONST;
    
    // 结构
    chineseToEnglish_["结构"] = TokenType::KW_STRUCT;
    chineseToEnglish_["类"] = TokenType::KW_STRUCT;
    chineseToEnglish_["枚举"] = TokenType::KW_ENUM;
    chineseToEnglish_["联合"] = TokenType::KW_UNION;
    
    // 特性
    chineseToEnglish_["特性"] = TokenType::KW_TRAIT;
    chineseToEnglish_["接口"] = TokenType::KW_TRAIT;
    chineseToEnglish_["实现"] = TokenType::KW_IMPL;
    
    // 布尔值
    chineseToEnglish_["真"] = TokenType::KW_TRUE;
    chineseToEnglish_["假"] = TokenType::KW_FALSE;
    chineseToEnglish_["空"] = TokenType::KW_NULL;
    
    // 异步
    chineseToEnglish_["异步"] = TokenType::KW_ASYNC;
    chineseToEnglish_["等待"] = TokenType::KW_AWAIT;
    
    // 模式匹配
    chineseToEnglish_["匹配"] = TokenType::KW_MATCH;
    chineseToEnglish_["模式"] = TokenType::KW_MATCH;
    
    // 类型
    chineseToEnglish_["整数"] = TokenType::TYPE_I32;
    chineseToEnglish_["浮点"] = TokenType::TYPE_F64;
    chineseToEnglish_["字符"] = TokenType::TYPE_CHAR;
    chineseToEnglish_["字符串"] = TokenType::TYPE_STRING;
    chineseToEnglish_["布尔"] = TokenType::TYPE_BOOL;
    chineseToEnglish_["无值"] = TokenType::TYPE_VOID;
    
    // 操作符
    chineseToEnglish_["加"] = TokenType::PLUS;
    chineseToEnglish_["减"] = TokenType::MINUS;
    chineseToEnglish_["乘"] = TokenType::STAR;
    chineseToEnglish_["除"] = TokenType::SLASH;
    chineseToEnglish_["模"] = TokenType::PERCENT;
    
    // 反向映射（英文到中文）
    for (const auto& pair : chineseToEnglish_) {
        englishToChinese_[pair.second] = pair.first;
    }
}

bool ChineseKeywordMap::isChineseKeyword(const std::string& word) const {
    return chineseToEnglish_.find(word) != chineseToEnglish_.end();
}

TokenType ChineseKeywordMap::getMappedType(const std::string& word) const {
    auto it = chineseToEnglish_.find(word);
    if (it != chineseToEnglish_.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::string ChineseKeywordMap::getEnglishKeyword(TokenType type) const {
    auto it = englishToChinese_.find(type);
    if (it != englishToChinese_.end()) {
        return it->second;
    }
    return "";
}

std::string SymbolProcessor::normalizeSymbol(const std::string& symbol) {
    std::string result;
    for (char c : symbol) {
        result += normalizePunctuation(c);
    }
    return result;
}

bool SymbolProcessor::isFullWidth(const std::string& symbol) {
    for (char c : symbol) {
        if (c >= 0xFF01 && c <= 0xFF5E) {
            return true;
        }
    }
    return false;
}

char SymbolProcessor::normalizeParenthesis(char c) {
    switch (c) {
        case 0xFF08: // （
            return '(';
        case 0xFF09: // ）
            return ')';
        case 0xFF3B: // ［
            return '[';
        case 0xFF3D: // ］
            return ']';
        case 0xFF5B: // ｛
            return '{';
        case 0xFF5D: // ｝
            return '}';
        default:
            return c;
    }
}

char SymbolProcessor::normalizePunctuation(char c) {
    // 转换全角符号为半角
    switch (c) {
        case 0x3002: // 。
            return '.';
        case 0xFF0C: // ，
            return ',';
        case 0xFF1A: // ：
            return ':';
        case 0xFF1B: // ；
            return ';';
        case 0xFF01: // ！
            return '!';
        case 0xFF1F: // ？
            return '?';
        case 0xFF08: // （
            return '(';
        case 0xFF09: // ）
            return ')';
        case 0xFF3B: // ［
            return '[';
        case 0xFF3D: // ］
            return ']';
        case 0xFF5B: // ｛
            return '{';
        case 0xFF5D: // ｝
            return '}';
        case 0xFF1C: // ＜
            return '<';
        case 0xFF1E: // ＞
            return '>';
        case 0x300C: // 「
        case 0x300E: // 」
            return '"';
        case 0x3001: // 、
            return ',';
        default:
            return c;
    }
}

std::string ChineseCommentParser::parseChineseDescription(const std::string& comment) {
    // 格式: [中文描述]
    size_t start = comment.find('[');
    size_t end = comment.find(']');
    
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return comment.substr(start + 1, end - start - 1);
    }
    
    return "";
}

bool ChineseCommentParser::validateChineseComment(const std::string& comment) {
    std::string desc = parseChineseDescription(comment);
    
    // 检查是否包含中文字符
    for (char c : desc) {
        if (c >= 0x4E00 && c <= 0x9FFF) { // 中文字符范围
            return true;
        }
    }
    
    return false;
}

std::string ChineseCommentParser::generateWarning(const std::string& functionName) {
    std::string warning = "警告: 函数 '" + functionName + 
                       "' 缺少中文描述注释。\n"
                       "请使用格式: fn " + functionName + "(...) -> type [中文描述] { ... }\n"
                       "或使用中文关键字: 计算 " + functionName + "(...) [中文描述] { ... }";
    return warning;
}

} // namespace syclang
