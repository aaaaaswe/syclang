# SysLang v4.0 快速开始指南

## 5 分钟快速入门

### 1. 克隆仓库

```bash
git clone https://github.com/username/syclang.git
cd syclang
```

### 2. 编译编译器

```bash
mkdir build && cd build
cmake -DBUILD_V4_FEATURES=ON ..
make -j$(nproc)
```

### 3. 编译第一个程序

创建 `hello.syl`:

```syolang
fn main() -> i32 {
    println("Hello, SysLang v4.0!");
    返回 0;
}
```

编译运行:

```bash
# x64
./syclang --arch x64 --output hello hello.syl
./hello

# WebAssembly (v3.0)
./syclang --target wasm32 --output hello.wasm hello.syl
```

### 4. 编译 EFI 操作系统

```bash
# 编译所有架构
bash scripts/build_efi_all.sh

# 或编译特定架构
./syclang --arch x64 --format efi --output bootx64.efi efi_system/efi_kernel.syl
```

### 5. 上传到 GitHub

```bash
bash scripts/upload_to_github.sh
```

## 示例代码集合

### 基础示例

#### Hello World
```syolang
fn main() -> i32 {
    println("Hello, World!");
    返回 0;
}
```

#### 斐波那契数列
```syolang
fn fibonacci(n: i32) -> i32 [斐波那契数列] {
    如果 n <= 1 {
        返回 n;
    }
    返回 fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() -> i32 {
    println(fibonacci(10));  // 输出: 55
    返回 0;
}
```

### v2.0 特性

#### 中文关键字
```syolang
计算 求和(甲: i32, 乙: i32) -> i32 [计算两个数的和] {
    返回 甲 + 乙;
}

fn main() -> i32 {
    println(求和(5, 3));  // 输出: 8
    返回 0;
}
```

#### 管道操作符
```syolang
fn main() -> i32 {
    let data = [1, 2, 3, 4, 5];
    
    let result = data
        |> 映射(|x| x * x)
        |> 过滤(|x| x > 5)
        |> 归约((a, b) => a + b, 0);
    
    println(result);  // 输出: 55 (9+16+25+36)
    返回 0;
}
```

#### 模式匹配
```syolang
fn 分类(数字: i32) -> string [分类数字] {
    匹配 数字 {
        0..10 => "小数字",
        11..100 => "中等数字",
        _ => "大数字"
    }
}
```

### v3.0 特性

#### Actor 模型
```syolang
#[演员]
#[邮箱容量(1000)]
struct 工作演员 {
    任务数: i32
    
    #[消息处理]
    fn 处理任务(任务: 任务) -> string {
        self.任务数 += 1;
        返回 format!("任务 {} 完成", 任务.id);
    }
}

fn 演员_示例() {
    let 系统 = ActorSystem::new("示例");
    let 演员 = 系统.创建演员(工作演员::new);
    演员.发送("处理任务", 任务 { id: 1 });
}
```

#### WebAssembly
```syolang
#[导出到WebAssembly]
fn 求平方(n: i32) -> i32 [求平方] {
    返回 n * n;
}
```

### v4.0 特性

#### 量子电路
```syolang
fn 量子传送() [量子传送] {
    let 爱丽丝 = 量子位::new();
    let 鲍勃 = 量子位::new();
    let mut 电路 = 量子电路::new(2);
    
    // 创建纠缠对
    电路.hadamard(&爱丽丝);
    电路.cnot(&爱丽丝, &鲍勃);
    
    返回 电路.执行();
}

fn grover_搜索(目标: i32) -> i32 [Grover搜索] {
    let 预言机 = grover_oracle(目标);
    let 迭代次数 = sqrt(16) as i32;
    
    运行 grover 算法(预言机, 迭代次数);
}
```

#### GPU 编程
```syolang
#[GPU内核]
fn 矩阵乘法(A: *f32, B: *f32, C: *f32, N: i32) {
    let 行 = 全局ID::x();
    let 列 = 全局ID::y();
    
    let mut 总和: f32 = 0.0;
    for k in 0..N {
        总和 += A[行 * N + k] * B[k * N + 列];
    }
    C[行 * N + 列] = 总和;
}
```

#### 形式化验证
```syolang
#[规范验证]
#[不变量条件("x >= 0")]
fn 绝对值(x: i32) -> i32 {
    如果 x < 0 {
        返回 -x;
    }
    返回 x;
}
```

## 编译命令速查

### 传统架构
```bash
# x64 (Windows/Linux/Unix)
./syclang --arch x64 --output app program.syl

# x86 (32位)
./syclang --arch x86 --output app program.syl

# ARM64 (Apple Silicon/Android)
./syclang --arch arm64 --output app program.syl

# ARM32
./syclang --arch arm32 --output app program.syl

# RISC-V64
./syclang --arch riscv64 --output app program.syl

# RISC-V32
./syclang --arch riscv32 --output app program.syl
```

### WebAssembly (v3.0)
```bash
# WASM 32-bit
./syclang --target wasm32 --output app.wasm program.syl

# WASM 64-bit
./syclang --target wasm64 --output app.wasm program.syl

# 生成 JavaScript 包装器
./syclang --target wasm32 --js-wrapper app.js program.syl
```

### 量子计算 (v4.0)
```bash
# 生成 QASM
./syclang --target quantum --output circuit.qasm program.syl

# 生成 Quil
./syclang --target quantum --format quil --output circuit.quil program.syl
```

### FPGA (v4.0)
```bash
# Xilinx FPGA
./syclang --target fpga --platform xilinx --output bitstream.bin program.syl

# Intel FPGA
./syclang --target fpga --platform intel --output bitstream.sof program.syl
```

### GPU (v4.0)
```bash
# CUDA
./syclang --target gpu --backend cuda --output kernel.cu program.syl

# ROCm
./syclang --target gpu --backend rocm --output kernel.hsaco program.syl

# OpenCL
./syclang --target gpu --backend opencl --output kernel.cl program.syl
```

## 运行程序

### ELF 程序
```bash
# 直接运行
./app

# 调试
gdb ./app
```

### WebAssembly

#### 浏览器
```html
<script>
    WebAssembly.instantiateStreaming(
        fetch('app.wasm')
    ).then(results => {
        const exports = results.instance.exports;
        exports.main();
    });
</script>
```

#### Node.js
```bash
node --experimental-wasi-unstable-preview1 app.wasm
```

#### Wasmtime
```bash
wasmtime app.wasm
```

### 量子电路
```bash
# 使用 Qiskit
python -c "
from qiskit import QuantumCircuit
qc = QuantumCircuit.from_qasm_file('circuit.qasm')
qc.draw('mpl')
"

# 模拟执行
python -c "
from qiskit import Aer, execute
backend = Aer.get_backend('qasm_simulator')
result = execute(qc, backend, shots=1000).result()
print(result.get_counts())
"
```

### FPGA
```bash
# 使用 Vivado
vivado -mode batch -source synth.tcl

# 加载到 FPGA
vivado -mode batch -source load_bitstream.tcl
```

### GPU
```bash
# CUDA
nvcc -arch=sm_70 kernel.cu -o kernel
./kernel

# OpenCL
clang -I/usr/include/CL kernel.cl -o kernel
./kernel
```

## 故障排除

### 编译错误

**Q: 找不到编译器？**
```bash
# 重新构建
cd build
cmake ..
make -j$(nproc)
```

**Q: 交叉编译失败？**
```bash
# Ubuntu
sudo apt-get install gcc-aarch64-linux-gnu gcc-riscv64-linux-gnu

# macOS
brew install aarch64-elf-gcc riscv64-elf-gcc
```

### 运行错误

**Q: WebAssembly 加载失败？**
```bash
# 确保浏览器支持
# 使用 wasmtime 测试
wasmtime app.wasm
```

**Q: 量子模拟出错？**
```bash
# 安装依赖
pip install qiskit qutip

# 测试安装
python -c "import qiskit; print(qiskit.__version__)"
```

## 下一步

- 阅读 [完整文档](README.md)
- 查看 [示例代码](examples/)
- 了解 [v3.0 特性](SYSLANG_V3.md)
- 了解 [v4.0 特性](SYSLANG_V4.md)
- 探索 [EFI 系统](efi_system/)

## 获取帮助

- GitHub Issues: https://github.com/username/syclang/issues
- Discussions: https://github.com/username/syclang/discussions
- Email: support@syclang.org

---

**祝你编程愉快！🚀**
