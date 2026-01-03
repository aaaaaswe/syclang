# SysLang 语言规范 v2.0

## 创新语法特性

### 1. 中文函数注释系统

SysLang 要求所有导出的函数必须提供中文描述：

```syolang
// 中文函数定义语法
fn calculate_sum(a: i32, b: i32) -> i32 [计算两个整数的和] {
    return a + b;
}

// 简写方式（使用中文关键词）
计算 计算和(甲: i32, 乙: i32) -> i32 [计算甲与乙的和] {
    返回 甲 + 乙;
}
```

### 2. 中文全角符号支持

完全支持中文全角符号和英文半角符号混合使用：

```syolang
// 使用全角符号
fn 示例（参数：i32）-> i32 {
    返回 参数；
}

// 混合使用
fn example(param: i32) -> i32 {
    return param;
}
```

### 3. 创新的函数式编程特性

管道操作符：

```syolang
fn process_data() {
    let data = [1, 2, 3, 4, 5];
    
    // 管道操作
    let result = data
        |> map(x => x * 2)
        |> filter(x => x > 5)
        |> reduce((a, b) => a + b, 0);
}
```

### 4. 模式匹配

```syolang
fn classify_number(n: i32) -> string {
    match n {
        0..10 => "小数字",
        11..100 => "中等数字",
        _ => "大数字"
    }
}
```

### 5. 结构化并发

```syolang
async fn download_files(urls: [string]) {
    for url in urls {
        async {
            let content = await http_get(url);
            save_file(content);
        }
    }
    await_all();
}
```

### 6. 属性系统

```syolang
#[内核函数]
#[优先级(高)]
fn schedule_process(proc: Process) [调度进程] {
    // 调度逻辑
}

#[内联]
fn fast_function(x: i32) -> i32 [快速函数] {
    return x * 2;
}
```

### 7. 平台特定代码

```syolang
#[平台(windows)]
fn get_current_dir() -> string [获取当前目录] {
    return windows_api::get_current_directory();
}

#[平台(linux)]
#[平台(unix)]
fn get_current_dir() -> string [获取当前目录] {
    return posix::getcwd();
}

#[平台(mac)]
fn get_current_dir() -> string [获取当前目录] {
    return bsd::getcwd();
}
```

### 8. 内存安全特性

智能指针和所有权：

```syolang
fn create_buffer(size: i32) -> unique_ptr<Buffer> [创建缓冲区] {
    let buf = unique_ptr::new(Buffer { size });
    return buf;
}

fn use_buffer() {
    let buf = create_buffer(1024);
    // buf 自动管理内存，无需手动释放
}
```

### 9. 声明式 UI

```syolang
#[ui窗口]
struct MainApp [主应用窗口] {
    title: "系统管理器"
    width: 800
    height: 600
    
    #[ui按钮]
    button_start: {
        label: "启动"
        position: (10, 10)
        size: (100, 40)
        onClick: start_service
    }
    
    #[ui列表]
    process_list: {
        columns: ["进程名", "PID", "CPU"]
        dataSource: process_manager::get_processes()
    }
}
```

### 10. 内核级原语

```syolang
#[中断处理程序]
fn timer_interrupt_handler() [定时器中断处理程序] {
    // 内核代码，直接操作硬件
    disable_interrupts();
    process_scheduler::tick();
    enable_interrupts();
}

#[内核模块]
#[初始化函数]
fn my_driver_init() -> i32 [驱动初始化] {
    register_device(&my_device);
    return 0;
}
```

## 类型系统

### 基础类型
- `i8, i16, i32, i64` - 有符号整数
- `u8, u16, u32, u64` - 无符号整数
- `f32, f64` - 浮点数
- `bool` - 布尔值
- `char` - 字符（支持 Unicode）
- `string` - 字符串
- `pointer<T>` - 指针类型

### 智能指针
- `unique_ptr<T>` - 独占所有权指针
- `shared_ptr<T>` - 共享所有权指针
- `weak_ptr<T>` - 弱引用指针

### 容器类型
- `[T]` - 数组
- `vector<T>` - 动态数组
- `map<K, V>` - 映射
- `set<T>` - 集合

## 关键字列表

### 中文关键字
- `计算` / `fn` - 函数定义
- `返回` / `return` - 返回值
- `如果` / `if` - 条件判断
- `否则` / `else` - 否则分支
- `循环` / `while` - 循环
- `匹配` / `match` - 模式匹配
- `异步` / `async` - 异步函数
- `等待` / `await` - 等待异步操作

### 英文关键字
- `fn` - 函数
- `let` - 变量声明
- `mut` - 可变变量
- `const` - 常量
- `struct` - 结构体
- `enum` - 枚举
- `union` - 联合
- `trait` - 特性/接口
- `impl` - 实现
- `where` - 约束条件

## 编译目标

### 支持的操作系统
- Windows (PE 格式)
- Linux (ELF 格式)
- Unix (ELF 格式)
- macOS (Mach-O 格式)

### 支持的架构
- x86-64 (AMD64)
- ARM64 (AArch64)
- RISC-V (计划中)
- MIPS (计划中)

## 编译器选项

```bash
# 编译为不同平台
syclang --target windows-x64 program.syl
syclang --target linux-x64 program.syl
syclang --target mac-arm64 program.syl

# 编译为内核模块
syclang --kernel --target linux-arm64 kernel.syl

# 编译为 EFI 应用
syclang --efi --output boot.efi efi_app.syl
```

## 标准库

### 核心库
- `core::memory` - 内存管理
- `core::sync` - 同步原语
- `core::io` - 输入输出

### 系统库
- `sys::process` - 进程管理
- `sys::thread` - 线程管理
- `sys::fs` - 文件系统
- `sys::net` - 网络通信

### GUI 库
- `gui::window` - 窗口管理
- `gui::widget` - UI 组件
- `gui::graphics` - 图形绘制

### 内核库
- `kernel::scheduler` - 进程调度
- `kernel::driver` - 设备驱动框架
- `kernel::mm` - 内存管理
- `kernel::vfs` - 虚拟文件系统
