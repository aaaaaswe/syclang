#include "syclang/optimizer/optimizer.h"
#include <iostream>

namespace syclang {

Optimizer::Optimizer() {
    optimizationLevel_ = 1;
}

void Optimizer::optimize(std::shared_ptr<IRModule> module) {
    std::cout << "Running optimizations...\n";
    
    // Dead code elimination
    if (optimizationLevel_ >= 1) {
        eliminateDeadCode(module);
    }
    
    // Constant folding
    if (optimizationLevel_ >= 1) {
        foldConstants(module);
    }
    
    // Common subexpression elimination
    if (optimizationLevel_ >= 2) {
        eliminateCommonSubexpressions(module);
    }
    
    // Loop optimizations
    if (optimizationLevel_ >= 2) {
        optimizeLoops(module);
    }
    
    std::cout << "Optimizations complete.\n";
}

void Optimizer::eliminateDeadCode(std::shared_ptr<IRModule> module) {
    // Remove unreachable code and unused variables
    for (auto& func : module->functions) {
        for (auto& block : func->blocks) {
            // Mark used instructions
            std::set<std::shared_ptr<IRInstruction>> used;
            for (auto& inst : block->instructions) {
                if (inst->opcode == Opcode::RET || inst->opcode == Opcode::STORE) {
                    used.insert(inst);
                }
            }
            
            // Remove unused instructions (simplified)
            auto newEnd = std::remove_if(block->instructions.begin(), 
                                        block->instructions.end(),
                                        [&used](const std::shared_ptr<IRInstruction>& inst) {
                                            return used.find(inst) == used.end();
                                        });
            block->instructions.erase(newEnd, block->instructions.end());
        }
    }
}

void Optimizer::foldConstants(std::shared_ptr<IRModule> module) {
    // Simplify constant expressions
    for (auto& func : module->functions) {
        for (auto& block : func->blocks) {
            for (auto& inst : block->instructions) {
                // Check if both operands are constants
                auto leftConst = std::dynamic_pointer_cast<IRConstant>(inst->operands[0]);
                auto rightConst = std::dynamic_pointer_cast<IRConstant>(inst->operands[1]);
                
                if (leftConst && rightConst) {
                    // Fold the operation
                    int64_t result = 0;
                    bool canFold = true;
                    
                    switch (inst->opcode) {
                        case Opcode::ADD:
                            result = leftConst->value_.intValue + rightConst->value_.intValue;
                            break;
                        case Opcode::SUB:
                            result = leftConst->value_.intValue - rightConst->value_.intValue;
                            break;
                        case Opcode::MUL:
                            result = leftConst->value_.intValue * rightConst->value_.intValue;
                            break;
                        case Opcode::DIV:
                            if (rightConst->value_.intValue != 0) {
                                result = leftConst->value_.intValue / rightConst->value_.intValue;
                            } else {
                                canFold = false;
                            }
                            break;
                        default:
                            canFold = false;
                    }
                    
                    if (canFold) {
                        // Replace instruction with constant
                        inst->operands.clear();
                        inst->operands.push_back(IRConstant::createI64(result));
                        inst->opcode = Opcode::LOAD; // Changed to load constant
                    }
                }
            }
        }
    }
}

void Optimizer::eliminateCommonSubexpressions(std::shared_ptr<IRModule> module) {
    // Identify and eliminate redundant computations
    // This is a simplified implementation
    for (auto& func : module->functions) {
        for (auto& block : func->blocks) {
            std::map<std::string, std::shared_ptr<IRValue>> valueMap;
            
            for (auto& inst : block->instructions) {
                if (inst->operands.size() >= 2) {
                    // Create a key for this expression
                    std::string key = std::to_string(static_cast<int>(inst->opcode));
                    for (const auto& op : inst->operands) {
                        key += ":" + op->toString();
                    }
                    
                    // Check if we've seen this expression before
                    auto it = valueMap.find(key);
                    if (it != valueMap.end()) {
                        // Replace with previous result
                        inst->result = it->second;
                    } else if (inst->result) {
                        // Store this result for future use
                        valueMap[key] = inst->result;
                    }
                }
            }
        }
    }
}

void Optimizer::optimizeLoops(std::shared_ptr<IRModule> module) {
    // Perform loop optimizations: invariant code motion, induction variable analysis, etc.
    // This is a placeholder for a full loop optimizer
    for (auto& func : module->functions) {
        for (auto& block : func->blocks) {
            // Identify loop headers and perform optimizations
            // Simplified: just mark loops for now
            if (block->name.find("while") != std::string::npos ||
                block->name.find("for") != std::string::npos) {
                // This is a loop block
            }
        }
    }
}

} // namespace syclang
