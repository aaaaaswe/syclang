#!/bin/bash

# SysLang 一键部署脚本
# 自动完成：构建 -> 测试 -> 编译 EFI 所有架构 -> 上传到 GitHub

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo -e "${MAGENTA}"
echo "████████████████████████████████████████████████████████████████████████████"
echo "█                                                                      █"
echo "█           SysLang v4.0 一键部署脚本                                  █"
echo "█           支持 11 种架构的完整 EFI 操作系统                          █"
echo "█                                                                      █"
echo "████████████████████████████████████████████████████████████████████████████"
echo -e "${NC}"
echo ""

# 步骤 1: 构建编译器
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}步骤 1/4: 构建 SysLang 编译器${NC}"
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo ""

if [ -f "$PROJECT_ROOT/build/syclang" ]; then
    echo -e "${GREEN}✓ 编译器已存在，跳过构建${NC}"
else
    echo "正在构建 SysLang 编译器..."
    cd "$PROJECT_ROOT"
    
    mkdir -p build
    cd build
    
    echo "配置 CMake..."
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_V3_FEATURES=ON \
        -DBUILD_V4_FEATURES=ON \
        ..
    
    echo "编译..."
    make -j$(nproc)
    
    cd "$PROJECT_ROOT"
    echo -e "${GREEN}✓ 编译器构建完成${NC}"
fi

echo ""

# 步骤 2: 运行测试
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}步骤 2/4: 运行测试${NC}"
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo ""

cd "$PROJECT_ROOT/build"

if [ -d "tests" ]; then
    echo "运行测试套件..."
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure --parallel $(nproc)
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ 所有测试通过${NC}"
        else
            echo -e "${YELLOW}⚠ 部分测试失败，继续执行...${NC}"
        fi
    else
        echo -e "${YELLOW}ctest 未找到，跳过测试${NC}"
    fi
else
    echo -e "${YELLOW}未找到测试目录，跳过测试${NC}"
fi

cd "$PROJECT_ROOT"
echo ""

# 步骤 3: 编译 EFI 所有架构
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}步骤 3/4: 编译 EFI 所有架构（11 种）${NC}"
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo ""

# 给脚本添加执行权限
chmod +x "$PROJECT_ROOT/scripts/build_efi_all.sh"

# 运行编译脚本
bash "$PROJECT_ROOT/scripts/build_efi_all.sh"

echo ""
echo -e "${GREEN}✓ EFI 编译完成${NC}"
echo ""

# 步骤 4: 上传到 GitHub
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}步骤 4/4: 上传到 GitHub${NC}"
echo -e "${BLUE}═════════════════════════════════════════════════════════════════${NC}"
echo ""

read -p "是否要上传到 GitHub? (y/n) " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    # 给脚本添加执行权限
    chmod +x "$PROJECT_ROOT/scripts/upload_to_github.sh"
    
    # 运行上传脚本
    bash "$PROJECT_ROOT/scripts/upload_to_github.sh"
else
    echo -e "${YELLOW}跳过上传${NC}"
fi

echo ""
echo -e "${MAGENTA}"
echo "████████████████████████████████████████████████████████████████████████████"
echo "█                                                                      █"
echo "█                 部署完成！                                          █"
echo "█                                                                      █"
echo "████████████████████████████████████████████████████████████████████████████"
echo -e "${NC}"
echo ""

# 显示构建摘要
echo "构建摘要:"
echo "================================"
echo ""

if [ -f "$PROJECT_ROOT/build/syclang" ]; then
    echo -e "${GREEN}✓${NC} SysLang 编译器"
    echo "  位置: $PROJECT_ROOT/build/syclang"
    echo ""
fi

if [ -d "$PROJECT_ROOT/build/efi" ]; then
    echo -e "${GREEN}✓${NC} EFI 内核（所有架构）"
    echo "  位置: $PROJECT_ROOT/build/efi/"
    echo ""
    echo "  编译的文件:"
    ls -lh "$PROJECT_ROOT/build/efi" 2>/dev/null | grep -E "\.(efi|wasm|qasm|bin|cu)$" | awk '{printf "    %-40s %10s\n", $9, $5}' || echo "    无 EFI 文件"
fi

echo ""
echo "使用方法:"
echo "================================"
echo ""

echo "编译 SysLang 程序:"
echo "  ./build/syclang --arch x64 --output app program.syl"
echo ""

echo "运行编译器:"
echo "  ./build/syclang --help"
echo ""

echo "查看示例:"
echo "  ls examples/"
echo ""

echo "阅读文档:"
echo "  cat README.md"
echo "  cat QUICKSTART.md"
echo ""

echo "EFI 系统使用:"
echo "  cat efi_system/README.md"
echo ""

echo -e "${CYAN}感谢使用 SysLang！${NC}"
echo ""

exit 0
