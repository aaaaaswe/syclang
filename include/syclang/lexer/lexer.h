#ifndef SYCLANG_LEXER_LEXER_H
#define SYCLANG_LEXER_LEXER_H

#include <string>
#include <vector>
#include "token.h"

namespace syclang {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    // Tokenize the entire source
    std::vector<Token> tokenize();
    
    // Get next token
    Token nextToken();
    
    // Peek at next token without consuming
    Token peekToken(size_t offset = 0);
    
private:
    const std::string& source_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    char peek(size_t offset = 0);
    char advance();
    void skipWhitespace();
    void skipComment();
    
    Token scanNumber();
    Token scanString();
    Token scanIdentifier();
    Token scanSymbol();
};

} // namespace syclang

#endif // SYCLANG_LEXER_LEXER_H
