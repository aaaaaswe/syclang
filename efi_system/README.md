# SysLang EFI 操作系统

这是一个完整的 EFI/UEFI 操作系统，使用 SysLang 和少量 C/C++ 编写。

## 系统功能

### 1. 编译器系统
- **SysLang 编译器**: 完整的 SysLang 编译器，支持中文语法
- **C/C++ 编译器**: 集成 GCC/Clang，支持编译 C/C++ 代码
- **实时编译**: 在 EFI 环境下实时编译代码

### 2. 分区管理系统
- **支持所有 Linux 分区格式**: ext4, BTRFS, XFS, JFS, ReiserFS
- **Windows 格式支持**: NTFS, FAT32, exFAT
- **分区操作**: 创建、删除、格式化分区
- **GPT 支持**: 完整的 GUID 分区表支持

### 3. 文件系统支持
- **ext4**: Linux 主流文件系统
- **NTFS**: Windows 文件系统（读/写）
- **FAT32**: 通用文件系统
- **BTRFS**: Linux 高级文件系统
- **XFS**: 企业级文件系统

### 4. GUI 系统
- **窗口管理器**: 完整的窗口系统
- **控件支持**: 按钮、文本框、列表、进度条、菜单
- **图形绘制**: 矩形、线条、文本、图像
- **事件处理**: 鼠标、键盘、窗口事件

### 5. 硬件驱动
- **显卡驱动**: 支持 VGA, EGA, VESA, PCIe 显卡
- **网卡驱动**: 支持常见以太网芯片
- **存储驱动**: AHCI/SATA, NVMe 支持
- **USB 驱动**: USB 2.0/3.0 支持

## 系统架构

```
SysLang EFI OS
├── 内核层 (Kernel Layer)
│   ├── 进程调度器 (Process Scheduler)
│   ├── 内存管理 (Memory Management)
│   ├── 中断处理 (Interrupt Handler)
│   └── 系统调用 (System Calls)
├── 驱动层 (Driver Layer)
│   ├── 显卡驱动 (Graphics Driver)
│   ├── 网卡驱动 (Network Driver)
│   ├── 存储驱动 (Storage Driver)
│   └── USB 驱动 (USB Driver)
├── 系统服务层 (System Services)
│   ├── 文件系统管理 (File System Manager)
│   ├── 分区管理 (Partition Manager)
│   ├── 网络协议栈 (Network Stack)
│   └── GUI 系统 (GUI System)
├── 编译器层 (Compiler Layer)
│   ├── SysLang 编译器
│   ├── C/C++ 编译器
│   └── 链接器
└── 应用层 (Application Layer)
    ├── 系统管理器 (System Manager)
    ├── 文件管理器 (File Manager)
    ├── 终端 (Terminal)
    └── 其他应用
```

## 编译和运行

### 构建系统
```bash
# 使用 SysLang 编译 EFI 系统
syclang --target efi --arch x64 --format efi \
        --output bootx64.efi efi_system/efi_kernel.syl

# 交叉编译（在 Linux 上编译 x64 EFI）
syclang --target windows-efi --arch x64 \
        --output bootx64.efi efi_system/efi_kernel.syl

# 交叉编译（在 x64 上编译 ARM64 EFI）
syclang --target linux-efi --arch arm64 \
        --output bootaa64.efi efi_system/efi_kernel.syl
```

### 部署到 UEFI
```bash
# 将 EFI 文件复制到 UEFI 分区
sudo mount /dev/sda1 /mnt/efi
sudo cp bootx64.efi /mnt/efi/EFI/BOOT/
sudo umount /mnt/efi

# 或使用 efibootmgr
sudo efibootmgr -c -d /dev/sda -p 1 -L "SysLang OS" -l /EFI/BOOT/bootx64.efi
```

## 使用说明

### 编译文件
1. 打开系统管理器
2. 点击"编译 SysLang"或"编译 C++"按钮
3. 选择源文件
4. 编译结果会显示在输出窗口

### 文件管理
1. 点击"文件管理"按钮
2. 浏览分区和文件
3. 支持拖拽操作

### 分区管理
1. 点击"分区管理"按钮
2. 查看所有分区信息
3. 可以创建、删除、格式化分区

## 中文支持

SysLang 完全支持中文：

```syolang
// 使用中文关键字
计算 求和(甲: i32, 乙: i32) -> i32 [计算两个数的和] {
    返回 甲 + 乙；
}

// 混合使用中文和英文
fn calculate_result(x: i32, y: i32) -> i32 [计算结果] {
    let result: i32 = x + y;
    返回 result;
}

// 全角符号
fn 示例（参数：i32）-> i32 {
    返回 参数；
}
```

## 开发计划

### v1.0 (已完成)
- ✅ 基本编译器框架
- ✅ 中文语法支持
- ✅ EFI 引导
- ✅ 基本 GUI
- ✅ 分区识别

### v2.0 (进行中)
- 🔄 完整文件系统支持
- 🔄 网络协议栈
- 🔄 更多硬件驱动

### v3.0 (计划中)
- 📋 多任务支持
- 📋 虚拟内存
- 📋 进程间通信
- 📋 POSIX 兼容层

## 许可证

MIT License

## 贡献

欢迎贡献！请阅读 CONTRIBUTING.md 了解详细信息。
