#!/bin/bash

# SysLang EFI 操作系统 - 跨架构编译脚本
# 支持 11 种架构的编译

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build/efi"
SYSLANG_BIN="$PROJECT_ROOT/build/syclang"

# 输出目录
OUTPUT_DIR="$BUILD_DIR/output"

# 源文件
SOURCE_FILE="$PROJECT_ROOT/efi_system/efi_kernel.syl"

# 创建构建目录
mkdir -p "$OUTPUT_DIR"

echo "================================================================"
echo "  SysLang EFI 操作系统 - 跨架构编译"
echo "  支持 11 种架构和 15 种文件系统"
echo "================================================================"
echo ""

# 检查编译器是否存在
if [ ! -f "$SYSLANG_BIN" ]; then
    echo -e "${RED}错误: SysLang 编译器未找到${NC}"
    echo "请先运行: mkdir -p build && cd build && cmake .. && make"
    exit 1
fi

echo -e "${BLUE}开始编译...${NC}"
echo ""

# 编译函数
compile_target() {
    local platform=$1
    local target_name=$2
    local output_file="$OUTPUT_DIR/efi_kernel_${target_name}"
    local extension=$3
    
    echo -e "${GREEN}[编译] $target_name${NC}"
    
    case $platform in
        x64)
            $SYSLANG_BIN --arch x64 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        x86)
            $SYSLANG_BIN --arch x86 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        arm64)
            $SYSLANG_BIN --arch arm64 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        arm32)
            $SYSLANG_BIN --arch arm32 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        riscv64)
            $SYSLANG_BIN --arch riscv64 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        riscv32)
            $SYSLANG_BIN --arch riscv32 --format efi \
                --output "$output_file.efi" \
                "$SOURCE_FILE"
            ;;
        wasm32)
            $SYSLANG_BIN --target wasm32 \
                --output "$output_file.wasm" \
                "$SOURCE_FILE"
            ;;
        wasm64)
            $SYSLANG_BIN --target wasm64 \
                --output "$output_file.wasm" \
                "$SOURCE_FILE"
            ;;
        quantum)
            $SYSLANG_BIN --target quantum \
                --output "$output_file.qasm" \
                "$SOURCE_FILE"
            ;;
        fpga)
            $SYSLANG_BIN --target fpga --platform xilinx \
                --output "$output_file.bin" \
                "$SOURCE_FILE"
            ;;
        gpu)
            $SYSLANG_BIN --target gpu --backend cuda \
                --output "$output_file.cu" \
                "$SOURCE_FILE"
            ;;
        *)
            echo -e "${RED}未知平台: $platform${NC}"
            return 1
            ;;
    esac
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ $target_name 编译成功${NC}"
        
        # 计算文件大小
        if [ -f "$output_file.$extension" ]; then
            size=$(ls -lh "$output_file.$extension" | awk '{print $5}')
            echo "  文件大小: $size"
        fi
    else
        echo -e "${RED}✗ $target_name 编译失败${NC}"
        return 1
    fi
    echo ""
}

# 编译所有架构
echo "================================================================"
echo "  开始编译所有目标平台"
echo "================================================================"
echo ""

# x64 平台
compile_target "x64" "x64" "efi"

# x86 平台
compile_target "x86" "x86" "efi"

# ARM64 平台
compile_target "arm64" "arm64" "efi"

# ARM32 平台
compile_target "arm32" "arm32" "efi"

# RISC-V64 平台
compile_target "riscv64" "riscv64" "efi"

# RISC-V32 平台
compile_target "riscv32" "riscv32" "efi"

# WebAssembly 32-bit (v3.0)
compile_target "wasm32" "wasm32" "wasm"

# WebAssembly 64-bit (v3.0)
compile_target "wasm64" "wasm64" "wasm"

# 量子电路 (v4.0)
compile_target "quantum" "quantum" "qasm"

# FPGA (v4.0)
compile_target "fpga" "fpga" "bin"

# GPU (v4.0)
compile_target "gpu" "gpu" "cu"

# 生成编译报告
echo "================================================================"
echo "  编译完成！生成文件列表:"
echo "================================================================"
echo ""

ls -lh "$OUTPUT_DIR" | grep -E "\.(efi|wasm|qasm|bin|cu)$" | awk '{printf "%-40s %10s\n", $9, $5}'

echo ""
echo "================================================================"
echo "  编译统计"
echo "================================================================"

total_files=$(ls -1 "$OUTPUT_DIR"/*.{efi,wasm,qasm,bin,cu} 2>/dev/null | wc -l)
total_size=$(du -sh "$OUTPUT_DIR" | cut -f1)

echo "  总文件数: $total_files"
echo "  总大小: $total_size"
echo ""

# 生成 SHA256 校验和
echo "================================================================"
echo "  文件校验和 (SHA256)"
echo "================================================================"
echo ""

for file in "$OUTPUT_DIR"/*.{efi,wasm,qasm,bin,cu}; do
    if [ -f "$file" ]; then
        filename=$(basename "$file")
        checksum=$(sha256sum "$file" | cut -d' ' -f1)
        echo "$checksum  $filename"
    fi
done

echo ""
echo "================================================================"
echo "  编译成功！所有平台 EFI 文件已生成"
echo "================================================================"
echo ""
echo "输出目录: $OUTPUT_DIR"
echo ""
echo "使用方法:"
echo "  x64/x86:  复制 EFI 文件到 ESP 分区的 /EFI/BOOT/ 目录"
echo "  ARM64:    复制到 UEFI 固件的 ARM64 启动分区"
echo "  WASM:     可在浏览器或 Node.js 中运行"
echo "  量子:     使用量子模拟器或真实量子计算机"
echo "  FPGA:     使用 Vivado/Quartus 综合生成位流"
echo "  GPU:      使用 nvcc 编译为 PTX/CUBIN"
echo ""

exit 0
