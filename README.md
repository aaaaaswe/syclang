# SysLang - System Level Programming Language

A modern system-level programming language that compiles to multiple architectures.

## Features

- **Multi-Architecture Support**: ARM64 and x64
- **System Programming**: Capable of writing kernel modules, drivers, and EFI applications
- **Inline Assembly**: Full inline assembly support for fine-grained hardware control
- **C/C++ Compatible**: Can link with existing C/C++ libraries
- **Zero-Cost Abstractions**: High-level syntax with low-level performance

## Quick Start

### Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Compiling a Program

```bash
# For x64
./syclang --arch x64 --output program.elf examples/hello.syl

# For ARM64
./syclang --arch arm64 --output program.elf examples/hello.syl
```

### Building EFI Application

```bash
./syclang --arch x64 --format efi --output bootx64.efi examples/efi_hello.syl
```

## Language Syntax

### Basic Structure

```syolang
// Entry point
fn main() -> i32 {
    print("Hello, World!\n");
    return 0;
}
```

### Inline Assembly

```syolang
fn add_numbers(a: i32, b: i32) -> i32 {
    let result: i32;
    asm volatile (
        "add %0, %1, %2"
        : "=r"(result)
        : "r"(a), "r"(b)
    );
    return result;
}
```

### Memory Management

```syolang
// Direct memory access
fn write_to_address(addr: *u8, value: u8) {
    *addr = value;
}
```

## EFI Development

SysLang provides built-in EFI support:

```syolang
extern fn efi_main(
    image_handle: *EFI_HANDLE,
    system_table: *EFI_SYSTEM_TABLE
) -> EFI_STATUS;
```

## Project Structure

```
syclang/
├── include/          # Headers
├── src/             # Source code
│   ├── lexer/       # Lexical analysis
│   ├── parser/      # Syntax parsing
│   ├── ir/          # Intermediate representation
│   └── codegen/     # Code generation
├── lib/             # Runtime library
├── examples/        # Example programs
└── docs/            # Documentation
```

## Requirements

- GCC/Clang with cross-compilation support
- CMake 3.15+
- For EFI: gnu-efi package

## License

MIT License

## Contributing

Contributions are welcome! Please read our contributing guidelines.
