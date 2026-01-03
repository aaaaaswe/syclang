#ifndef SYCLANG_SYMBOL_TABLE_H
#define SYCLANG_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace syclang {

enum class SymbolKind {
    VARIABLE,
    FUNCTION,
    TYPE,
    CONSTANT
};

struct SymbolInfo {
    SymbolKind kind;
    std::string type;
    void* data; // Associated data (AST node, etc.)
};

struct Scope {
    std::string name;
    std::map<std::string, SymbolInfo> symbols;
};

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
    // Scope management
    void enterScope(const std::string& name = "");
    void exitScope();
    
    // Symbol operations
    void define(const std::string& name, SymbolInfo info);
    bool lookup(const std::string& name, SymbolInfo& info);
    bool lookupInCurrentScope(const std::string& name, SymbolInfo& info);
    
    // Debug
    void print() const;
    
private:
    std::vector<std::shared_ptr<Scope>> scopes_;
};

} // namespace syclang

#endif // SYCLANG_SYMBOL_TABLE_H
