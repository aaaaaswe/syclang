#ifndef SYCLANG_OPTIMIZER_OPTIMIZER_H
#define SYCLANG_OPTIMIZER_OPTIMIZER_H

#include "syclang/ir/ir.h"
#include <memory>
#include <set>
#include <map>

namespace syclang {

class Optimizer {
public:
    Optimizer();
    
    void setOptimizationLevel(int level) { optimizationLevel_ = level; }
    
    // Run all optimizations
    void optimize(std::shared_ptr<IRModule> module);
    
private:
    int optimizationLevel_;
    
    // Individual optimizations
    void eliminateDeadCode(std::shared_ptr<IRModule> module);
    void foldConstants(std::shared_ptr<IRModule> module);
    void eliminateCommonSubexpressions(std::shared_ptr<IRModule> module);
    void optimizeLoops(std::shared_ptr<IRModule> module);
};

} // namespace syclang

#endif // SYCLANG_OPTIMIZER_OPTIMIZER_H
