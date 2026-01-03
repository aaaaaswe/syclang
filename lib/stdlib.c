// SysLang Runtime Library

// Simple I/O functions
void print_char(char c) {
    // Platform-specific implementation
    // For x64 Linux:
    __asm__ volatile (
        "mov $1, %%rax\n"
        "mov $1, %%rdi\n"
        "mov %0, %%rsi\n"
        "mov $1, %%rdx\n"
        "syscall"
        :
        : "r"(&c)
        : "rax", "rdi", "rsi", "rdx"
    );
}

void print_string(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    
    // Platform-specific implementation
    // For x64 Linux:
    __asm__ volatile (
        "mov $1, %%rax\n"
        "mov $1, %%rdi\n"
        "mov %0, %%rsi\n"
        "mov %1, %%rdx\n"
        "syscall"
        :
        : "r"(str), "r"(len)
        : "rax", "rdi", "rsi", "rdx"
    );
}

void print_int(int n) {
    char buffer[20];
    int i = 0;
    
    if (n == 0) {
        print_char('0');
        return;
    }
    
    if (n < 0) {
        print_char('-');
        n = -n;
    }
    
    while (n > 0) {
        buffer[i++] = '0' + (n % 10);
        n /= 10;
    }
    
    while (i > 0) {
        print_char(buffer[--i]);
    }
}

void* allocate(int size) {
    // Simple allocation - in real implementation, use sbrk or similar
    // For now, return null
    return (void*)0;
}

void deallocate(void* ptr) {
    // Free memory
    (void)ptr;
}
