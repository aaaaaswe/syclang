# Windows 编译修复指南

## 问题分析

你的代码版本较旧，存在以下主要问题：

1. **头文件 include 路径错误**：缺少 `syclang/` 前缀
2. **缺失枚举值**：TokenType 缺少 `KW_EXTERN`
3. **缺失构造函数**：IRConstant 和 IRVariable 缺少类型参数构造函数
4. **缺失运算符重载**：IRType 缺少输出运算符
5. **Lexer 类型转换错误**

## 修复方法

### 方法 1：从 GitHub 重新下载（推荐）

直接从最新版本下载：

```cmd
cd C:\Users\qqqqq\Downloads
git clone https://github.com/aaaaaswe/syclang.git syclang-latest
cd syclang-latest
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build . -j8
```

### 方法 2：手动修复现有代码

如果必须使用现有代码，请按以下步骤修复：

#### 1. 修复所有头文件 include 路径

需要在所有 `.h` 和 `.cpp` 文件中修改 include 语句：

**错误格式：**
```cpp
#include "lexer/lexer.h"
#include "parser/ast.h"
#include "lexer/token.h"
```

**正确格式：**
```cpp
#include "syclang/lexer/lexer.h"
#include "syclang/parser/ast.h"
#include "syclang/lexer/token.h"
```

**需要修改的文件：**
- `include/syclang/lexer/chinese_lexer.h`
- `include/syclang/ir/ir_generator.h`
- `include/syclang/parser/parser.h`
- `include/syclang/parser/ast.h`
- 以及所有源文件

#### 2. 添加缺失的枚举值

在 `include/syclang/lexer/token.h` 的 `TokenType` 枚举中添加：

```cpp
enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_MUT, KW_CONST,
    KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_RETURN, KW_STRUCT, KW_ENUM, KW_UNION, KW_EXTERN,  // 确保包含 KW_EXTERN
    // ...
};
```

#### 3. 添加 IRType 输出运算符

在 `include/syclang/ir/ir.h` 文件末尾（namespace syclang 闭合前）添加：

```cpp
// IRType 输出操作符
std::ostream& operator<<(std::ostream& os, IRType type);
```

在 `src/ir/ir.cpp` 文件开头添加实现：

```cpp
#include "syclang/ir/ir.h"
#include <sstream>

namespace syclang {

// IRType 输出操作符
std::ostream& operator<<(std::ostream& os, IRType type) {
    switch (type) {
        case IRType::I8: os << "i8"; break;
        case IRType::I16: os << "i16"; break;
        case IRType::I32: os << "i32"; break;
        case IRType::I64: os << "i64"; break;
        case IRType::U8: os << "u8"; break;
        case IRType::U16: os << "u16"; break;
        case IRType::U32: os << "u32"; break;
        case IRType::U64: os << "u64"; break;
        case IRType::F32: os << "f32"; break;
        case IRType::F64: os << "f64"; break;
        case IRType::BOOL: os << "bool"; break;
        case IRType::VOID: os << "void"; break;
        case IRType::POINTER: os << "ptr"; break;
        default: os << "unknown"; break;
    }
    return os;
}
```

#### 4. 修复 IRConstant 和 IRVariable 构造函数

在 `include/syclang/ir/ir.h` 中确保有：

```cpp
class IRConstant : public IRValue {
public:
    union {
        int64_t intValue;
        uint64_t uintValue;
        double floatValue;
    } value_;
    
    IRConstant() : IRValue() {}
    explicit IRConstant(IRType type) : IRValue(type) {}  // 必须有这个
    
    // ...
};

class IRVariable : public IRValue {
public:
    std::string name;
    int registerNum;
    bool isGlobal;
    int offset;
    
    IRVariable() : IRValue() {}
    explicit IRVariable(IRType type) : IRValue(type) {}  // 必须有这个
    
    // ...
};
```

#### 5. 修复 Lexer 转换问题

在 `src/lexer/lexer.cpp` 中，找到 `peekToken` 函数，修改：

```cpp
// 错误的代码：
Token token = *this;

// 正确的代码：
Token token = nextToken();  // 或者创建一个新的 Token
```

## 快速修复脚本

如果你有 sed 或 PowerShell，可以使用以下脚本批量修复：

### PowerShell 修复脚本 (Windows)

```powershell
# 修复头文件路径
Get-ChildItem -Recurse -Include *.h,*.cpp | ForEach-Object {
    $content = Get-Content $_.FullName -Raw
    $content = $content -replace '#include "lexer/', '#include "syclang/lexer/'
    $content = $content -replace '#include "parser/', '#include "syclang/parser/'
    $content = $content -replace '#include "ir/', '#include "syclang/ir/'
    $content = $content -replace '#include "codegen/', '#include "syclang/codegen/'
    $content = $content -replace '#include "symbol_table.h"', '#include "syclang/symbol_table.h"'
    Set-Content $_.FullName $content -NoNewline
    Write-Host "Fixed: $($_.FullName)"
}
```

### 使用说明

1. 将上述脚本保存为 `fix_includes.ps1`
2. 在 `C:\Users\qqqqq\Downloads\syclang-main` 目录下运行：
   ```powershell
   powershell -ExecutionPolicy Bypass -File fix_includes.ps1
   ```

3. 然后手动修复其他问题（参考上面的详细说明）

4. 重新编译：
   ```cmd
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build . -j8
   ```

## 验证修复

编译成功后，应该会看到：

```
[100%] Built target syclang
```

然后可以测试：

```cmd
syclang.exe --help
```

## 推荐方案

**强烈建议使用方法 1（从 GitHub 重新下载）**，因为：

1. 代码已经修复了所有编译问题
2. 包含了完整的功能实现
3. 更新更容易，直接 git pull 即可

手动修复容易遗漏问题，且后续维护困难。
