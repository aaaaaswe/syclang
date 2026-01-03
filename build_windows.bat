@echo off
REM SysLang Windows 编译脚本
REM 自动检测编译环境并编译 SysLang

setlocal EnableDelayedExpansion

echo.
echo ========================================================================
echo          SysLang v4.0.0 - Windows 编译脚本
echo ========================================================================
echo.

REM 检查 CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 CMake！
    echo.
    echo 请先安装 CMake:
    echo 1. 从 https://cmake.org/download/ 下载并安装
    echo 2. 或使用 Visual Studio Installer 安装 CMake 工具
    echo.
    pause
    exit /b 1
)

echo [✓] 找到 CMake
cmake --version
echo.

REM 检查编译器
set HAS_CL=0
set HAS_GCC=0

REM 检查 MSVC (Visual Studio)
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set HAS_CL=1
    echo [✓] 找到 MSVC 编译器 (Visual Studio)
)

REM 检查 GCC (MinGW)
where gcc >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set HAS_GCC=1
    echo [✓] 找到 GCC 编译器 (MinGW)
)

echo.

if %HAS_CL%==0 if %HAS_GCC%==0 (
    echo [错误] 未找到任何编译器！
    echo.
    echo 请先安装编译器之一:
    echo.
    echo 选项 1: Visual Studio 2022
    echo   - 下载: https://visualstudio.microsoft.com/downloads/
    echo   - 安装 "使用 C++ 的桌面开发" 工作负载
    echo.
    echo 选项 2: MinGW-w64
    echo   - 下载: https://www.msys2.org/
    echo   - 在 MSYS2 终端运行: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
    echo.
    pause
    exit /b 1
)

REM 选择编译器
if %HAS_CL%==1 (
    set COMPILER=Visual Studio
    set GENERATOR=-G "Visual Studio 17 2022" -A x64
) else (
    set COMPILER=MinGW
    set GENERATOR=-G "MinGW Makefiles"
)

echo 使用编译器: %COMPILER%
echo.

REM 清理旧的构建目录
if exist build (
    echo [清理] 删除旧的构建目录...
    rmdir /s /q build
)

REM 创建构建目录
echo [创建] 构建目录...
mkdir build
cd build

REM 配置 CMake
echo.
echo ========================================================================
echo           步骤 1/2: 配置 CMake
echo ========================================================================
echo.

cmake .. -DCMAKE_BUILD_TYPE=Release %GENERATOR%

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [错误] CMake 配置失败！
    echo.
    pause
    exit /b 1
)

echo.
echo [✓] CMake 配置成功
echo.

REM 编译
echo.
echo ========================================================================
echo           步骤 2/2: 编译 SysLang
echo ========================================================================
echo.

cmake --build . --config Release -j8

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [错误] 编译失败！
    echo.
    pause
    exit /b 1
)

echo.
echo [✓] 编译成功
echo.

REM 查找可执行文件
if exist Release\syclang.exe (
    set EXE_PATH=Release\syclang.exe
) else if exist syclang.exe (
    set EXE_PATH=syclang.exe
) else (
    echo [警告] 未找到编译好的 syclang.exe
    pause
    exit /b 1
)

echo.
echo ========================================================================
echo           编译完成！
echo ========================================================================
echo.
echo 可执行文件: !EXE_PATH!
echo 文件大小:
dir "!EXE_PATH!" | findstr "syclang.exe"
echo.

REM 测试编译器
echo [测试] 运行 syclang.exe --help
echo.
"!EXE_PATH!" --help
if %ERRORLEVEL% EQU 0 (
    echo.
    echo [✓] 编译器测试成功！
) else (
    echo.
    echo [警告] 编译器测试失败，但编译已完成
)

echo.
echo ========================================================================
echo           创建发布包
echo ========================================================================
echo.

REM 创建发布目录
if not exist release mkdir release
if not exist release\syclang-v4.0.0-windows-x64 mkdir release\syclang-v4.0.0-windows-x64

REM 复制文件
copy "!EXE_PATH!" release\syclang-v4.0.0-windows-x64\ >nul
copy ..\README.md release\syclang-v4.0.0-windows-x64\ >nul 2>&1
copy ..\LICENSE release\syclang-v4.0.0-windows-x64\ >nul 2>&1
copy ..\CONTRIBUTING.md release\syclang-v4.0.0-windows-x64\ >nul 2>&1
copy ..\QUICKSTART.md release\syclang-v4.0.0-windows-x64\ >nul 2>&1

REM 复制示例
xcopy /E /I /Y ..\examples release\syclang-v4.0.0-windows-x64\examples >nul 2>&1

echo [✓] 发布文件已复制到 release\syclang-v4.0.0-windows-x64\
echo.

REM 创建 ZIP 包
where powershell >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [创建] ZIP 压缩包...
    powershell -Command "Compress-Archive -Path 'release\syclang-v4.0.0-windows-x64' -DestinationPath 'release\syclang-v4.0.0-windows-x64.zip' -Force"
    if %ERRORLEVEL% EQU 0 (
        echo [✓] ZIP 包创建成功: release\syclang-v4.0.0-windows-x64.zip
        dir release\*.zip | findstr "zip"
    )
)

echo.
echo ========================================================================
echo           编译摘要
echo ========================================================================
echo.
echo 项目: SysLang v4.0.0
echo 编译器: %COMPILER%
echo 平台: Windows x64
echo.
echo 文件:
echo   可执行文件: !EXE_PATH!
if exist release\syclang-v4.0.0-windows-x64.zip (
    echo   发布包:      release\syclang-v4.0.0-windows-x64.zip
)
echo.
echo 使用方法:
echo   cd build
echo   "!EXE_PATH!" --help
echo   "!EXE_PATH!" ..\examples\hello.syl --output app.s
echo.
echo ========================================================================
echo.

pause
