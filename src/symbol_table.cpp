#include "syclang/symbol_table.h"
#include <iostream>

namespace syclang {

SymbolTable::SymbolTable() {
    // Create global scope
    scopes_.push_back(std::make_shared<Scope>());
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::enterScope(const std::string& name) {
    auto scope = std::make_shared<Scope>();
    scope->name = name;
    scopes_.push_back(scope);
}

void SymbolTable::exitScope() {
    if (scopes_.size() > 1) {
        scopes_.pop_back();
    }
}

void SymbolTable::define(const std::string& name, SymbolInfo info) {
    if (scopes_.empty()) {
        std::cerr << "Error: No active scope\n";
        return;
    }
    scopes_.back()->symbols[name] = info;
}

bool SymbolTable::lookup(const std::string& name, SymbolInfo& info) {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto symIt = (*it)->symbols.find(name);
        if (symIt != (*it)->symbols.end()) {
            info = symIt->second;
            return true;
        }
    }
    return false;
}

bool SymbolTable::lookupInCurrentScope(const std::string& name, SymbolInfo& info) {
    if (scopes_.empty()) {
        return false;
    }
    
    auto symIt = scopes_.back()->symbols.find(name);
    if (symIt != scopes_.back()->symbols.end()) {
        info = symIt->second;
        return true;
    }
    return false;
}

void SymbolTable::print() const {
    std::cout << "Symbol Table:\n";
    for (size_t i = 0; i < scopes_.size(); ++i) {
        std::cout << "  Scope " << i << " (" << scopes_[i]->name << "):\n";
        for (const auto& sym : scopes_[i]->symbols) {
            std::cout << "    " << sym.first << ": ";
            switch (sym.second.kind) {
                case SymbolKind::VARIABLE: std::cout << "variable"; break;
                case SymbolKind::FUNCTION: std::cout << "function"; break;
                case SymbolKind::TYPE: std::cout << "type"; break;
                case SymbolKind::CONSTANT: std::cout << "constant"; break;
            }
            std::cout << "\n";
        }
    }
}

} // namespace syclang
