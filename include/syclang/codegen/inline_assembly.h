#ifndef SYCLANG_CODEGEN_INLINE_ASSEMBLY_H
#define SYCLANG_CODEGEN_INLINE_ASSEMBLY_H

#include "ir/ir.h"
#include <string>
#include <vector>

namespace syclang {

std::string processInlineAssembly(const std::string& asmString, bool isVolatile,
                                   const std::vector<std::string>& clobbers,
                                   Architecture arch);

} // namespace syclang

#endif // SYCLANG_CODEGEN_INLINE_ASSEMBLY_H
