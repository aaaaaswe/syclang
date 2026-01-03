# Windows 编译指南

本指南介绍如何在 Windows 上编译 SysLang。

## 前置要求

### 选项 1: 使用 Visual Studio (推荐)

1. 安装 [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
   - 下载 "Community" 免费版本
   - 安装时选择 **"使用 C++ 的桌面开发"** 工作负载
   - 确保勾选以下组件：
     - MSVC v143 - VS 2022 C++ x64/x86 生成工具
     - CMake 工具
     - Windows 10 SDK（或 Windows 11 SDK）

### 选项 2: 使用 MinGW-w64

1. 下载并安装 [MSYS2](https://www.msys2.org/)
   - 运行安装程序（默认安装到 `C:\msys64`）
2. 打开 "MSYS2 MinGW 64-bit" 终端
3. 安装编译工具：
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make
   ```
4. 将 `C:\msys64\mingw64\bin` 添加到系统 PATH

### 选项 3: 使用预编译的二进制包（最快）

直接从 GitHub Release 下载预编译的版本。

## 编译步骤

### 方法 1: 使用 Visual Studio (GUI)

1. 双击打开 `CMakeLists.txt`，Visual Studio 会自动创建一个 CMake 缓存
2. 选择 **x64-Release** 配置
3. 点击 **生成** -> **全部生成**
4. 编译完成后，可执行文件在 `build\Release\` 目录

### 方法 2: 使用命令行 (Visual Studio)

1. 打开 "x64 Native Tools Command Prompt for VS 2022"
2. 切换到 SysLang 源码目录：
   ```cmd
   cd C:\path\to\syclang
   ```
3. 创建构建目录并编译：
   ```cmd
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release
   ```
4. 编译完成后，可执行文件为 `syclang.exe`

### 方法 3: 使用 MinGW-w64

1. 打开 "MSYS2 MinGW 64-bit" 终端
2. 切换到 SysLang 源码目录：
   ```bash
   cd /c/path/to/syclang
   ```
3. 创建构建目录并编译：
   ```bash
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
   cmake --build . -j8
   ```
4. 编译完成后，可执行文件为 `syclang.exe`

### 方法 4: 使用编译脚本（推荐）

我们提供了一个自动化脚本：

```cmd
build_windows.bat
```

该脚本会自动：
- 检查编译环境
- 运行 CMake 配置
- 编译项目
- 创建发布包

## 编译输出

编译成功后，你会得到：

```
build/
└── Release/
    └── syclang.exe        # SysLang 编译器可执行文件
```

或

```
build/
└── syclang.exe            # SysLang 编译器可执行文件
```

## 运行编译器

### Windows 命令行

```cmd
cd build\Release
syclang.exe --help
```

或

```cmd
cd build
syclang.exe --help
```

### 编译示例程序

```cmd
# 编译 hello.syl
syclang.exe examples\hello.syl --output app.s

# 编译 EFI 程序
syclang.exe examples\efi_hello.syl --format efi --arch x64 --output boot.efi
```

## 常见问题

### Q1: 找不到 MSVC 编译器

**解决方法**:
1. 确保 Visual Studio 已正确安装
2. 使用 "x64 Native Tools Command Prompt" 而不是普通 CMD
3. 或在 CMD 中运行：
   ```cmd
   call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
   ```

### Q2: CMake 找不到

**解决方法**:
1. 确保 CMake 已安装
2. 将 CMake 的 bin 目录添加到 PATH：
   - Visual Studio 安装: `C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin`
   - 独立安装: `C:\Program Files\CMake\bin`

### Q3: 链接错误

**解决方法**:
1. 清理构建目录：
   ```cmd
   rmdir /s /q build
   ```
2. 重新运行 CMake 配置

### Q4: 编译后运行提示缺少 DLL

**解决方法**:
- Visual Studio: 编译的是静态链接版本，不应该有此问题
- MinGW: 可能需要将 MinGW 的 DLL 复制到编译目录，或添加 MinGW bin 到 PATH

## 验证安装

运行以下命令验证编译器是否正常工作：

```cmd
syclang.exe --version
```

或

```cmd
syclang.exe --help
```

如果显示帮助信息，说明编译成功！

## 下一步

- 阅读 [QUICKSTART.md](QUICKSTART.md) 了解基本用法
- 查看 [examples/](examples/) 目录中的示例程序
- 阅读完整文档 [README.md](README.md)

## 需要帮助？

如果遇到编译问题，请在 GitHub 提交 Issue：
https://github.com/aaaaaswe/syclang/issues
