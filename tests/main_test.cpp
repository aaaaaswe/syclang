#include "syclang/lexer/lexer.h"
#include "syclang/parser/parser.h"
#include "syclang/ir/ir_generator.h"
#include <iostream>
#include <cassert>

void test_lexer() {
    std::cout << "Testing Lexer...\n";
    
    std::string source = "fn main() -> i32 { return 0; }";
    syclang::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    assert(!tokens.empty());
    assert(tokens[0].is(syclang::TokenType::KW_FN));
    
    std::cout << "  Lexer tests passed!\n";
}

void test_parser() {
    std::cout << "Testing Parser...\n";
    
    std::string source = "fn main() -> i32 { return 0; }";
    syclang::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    syclang::Parser parser(tokens);
    auto program = parser.parse();
    
    assert(program != nullptr);
    assert(!program->declarations.empty());
    
    std::cout << "  Parser tests passed!\n";
}

void test_ir_generation() {
    std::cout << "Testing IR Generation...\n";
    
    std::string source = "fn main() -> i32 { return 0; }";
    syclang::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    syclang::Parser parser(tokens);
    auto program = parser.parse();
    
    syclang::IRGenerator irGen(syclang::Architecture::X64);
    auto module = irGen.generate(program);
    
    assert(module != nullptr);
    assert(!module->functions.empty());
    
    std::cout << "  IR Generation tests passed!\n";
}

int main() {
    std::cout << "Running SysLang Tests\n";
    std::cout << "=====================\n\n";
    
    try {
        test_lexer();
        test_parser();
        test_ir_generation();
        
        std::cout << "\nAll tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed: " << e.what() << "\n";
        return 1;
    }
}
