#ifndef SYCLANG_LEXER_CHINESE_LEXER_H
#define SYSLANG_LEXER_CHINESE_LEXER_H

#include "syclang/lexer/lexer.h"
#include "syclang/lexer/token.h"
#include <string>
#include <unordered_map>

namespace syclang {

// 中文关键字映射
class ChineseKeywordMap {
public:
    static ChineseKeywordMap& getInstance();
    
    // 检查是否是中文关键字
    bool isChineseKeyword(const std::string& word) const;
    
    // 将中文关键字转换为英文对应的 TokenType
    TokenType getMappedType(const std::string& word) const;
    
    // 获取中英文映射
    std::string getEnglishKeyword(TokenType type) const;
    
private:
    ChineseKeywordMap();
    void initializeMap();
    
    std::unordered_map<std::string, TokenType> chineseToEnglish_;
    std::unordered_map<TokenType, std::string> englishToChinese_;
};

// 符号处理器（全角/半角）
class SymbolProcessor {
public:
    // 转换全角符号为半角
    static std::string normalizeSymbol(const std::string& symbol);
    
    // 检查是否是全角符号
    static bool isFullWidth(const std::string& symbol);
    
    // 转换全角括号
    static char normalizeParenthesis(char c);
    
    // 转换全角标点
    static char normalizePunctuation(char c);
};

// 中文函数注释解析器
class ChineseCommentParser {
public:
    struct FunctionInfo {
        std::string name;
        std::string chineseDescription;
        bool hasChineseDescription;
    };
    
    // 解析中文注释 [中文描述]
    static std::string parseChineseDescription(const std::string& comment);
    
    // 验证函数是否有中文注释
    static bool validateChineseComment(const std::string& comment);
    
    // 生成警告信息
    static std::string generateWarning(const std::string& functionName);
};

} // namespace syclang

#endif // SYCLANG_LEXER_CHINESE_LEXER_H
