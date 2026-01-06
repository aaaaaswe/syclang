# SysLang Windows 编译修复脚本
# 自动修复头文件 include 路径问题

Write-Host "======================================" -ForegroundColor Cyan
Write-Host " SysLang 头文件路径修复脚本" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# 检查是否在正确目录
$currentDir = Get-Location
Write-Host "当前目录: $currentDir" -ForegroundColor Yellow
Write-Host ""

# 查找所有需要修改的文件
Write-Host "正在查找需要修改的文件..." -ForegroundColor Yellow
$files = Get-ChildItem -Recurse -Include *.h,*.cpp | Where-Object {
    $_.FullName -notmatch "\\build\\" -and
    $_.FullName -notmatch "\\release\\"
}

Write-Host "找到 $($files.Count) 个文件" -ForegroundColor Green
Write-Host ""

$fixedCount = 0
$errorCount = 0

foreach ($file in $files) {
    try {
        $content = Get-Content $file.FullName -Raw
        $originalContent = $content

        # 修复头文件路径
        $content = $content -replace '#include "lexer/', '#include "syclang/lexer/'
        $content = $content -replace '#include "parser/', '#include "syclang/parser/'
        $content = $content -replace '#include "ir/', '#include "syclang/ir/'
        $content = $content -replace '#include "codegen/', '#include "syclang/codegen/'
        $content = $content -replace '#include "optimizer/', '#include "syclang/optimizer/'
        $content = $content -replace '#include "symbol_table.h"', '#include "syclang/symbol_table.h"'
        $content = $content -replace '#include "token.h"', '#include "syclang/lexer/token.h"'
        $content = $content -replace '#include "ast.h"', '#include "syclang/parser/ast.h"'

        # 检查是否有修改
        if ($content -ne $originalContent) {
            Set-Content $file.FullName $content -NoNewline
            $relativePath = $file.FullName.Substring($currentDir.Path.Length + 1)
            Write-Host "[FIXED] $relativePath" -ForegroundColor Green
            $fixedCount++
        }
    }
    catch {
        Write-Host "[ERROR] $($_.FullName): $($_.Exception.Message)" -ForegroundColor Red
        $errorCount++
    }
}

Write-Host ""
Write-Host "======================================" -ForegroundColor Cyan
Write-Host " 修复完成" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "已修复文件: $fixedCount" -ForegroundColor Green
if ($errorCount -gt 0) {
    Write-Host "错误文件: $errorCount" -ForegroundColor Red
}
Write-Host ""
Write-Host "接下来请手动修复以下问题：" -ForegroundColor Yellow
Write-Host "1. 在 token.h 中添加 KW_EXTERN 枚举值" -ForegroundColor Yellow
Write-Host "2. 在 ir.h 中添加 IRType 输出运算符声明" -ForegroundColor Yellow
Write-Host "3. 在 ir.cpp 中添加 IRType 输出运算符实现" -ForegroundColor Yellow
Write-Host "4. 确保 IRConstant 和 IRVariable 有 explicit 构造函数" -ForegroundColor Yellow
Write-Host "5. 修复 Lexer::peekToken 中的类型转换错误" -ForegroundColor Yellow
Write-Host ""
Write-Host "详细说明请参考 WINDOWS_FIX.md" -ForegroundColor Yellow
Write-Host ""

if ($errorCount -eq 0) {
    Write-Host "脚本执行成功！" -ForegroundColor Green
} else {
    Write-Host "脚本执行完成，但有错误！" -ForegroundColor Yellow
}
