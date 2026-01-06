#ifndef SYCLANG_IR_PLATFORM_GENERATOR_H
#define SYCLANG_IR_PLATFORM_GENERATOR_H

#include "syclang/ir/ir.h"
#include <string>
#include <memory>

namespace syclang {

// Platform enum
enum class Platform {
    WINDOWS,
    LINUX,
    UNIX,
    MACOS,
    FREEBSD,
    OPENBSD,
    NETBSD
};

// Binary format enum
enum class BinaryFormat {
    PE,      // Windows (Portable Executable)
    ELF,     // Linux/Unix (Executable and Linkable Format)
    MACH_O,  // macOS
    RAW      // Raw binary for kernels
};

struct PlatformTarget {
    Platform platform;
    Architecture arch;
    BinaryFormat format;
    std::string triple;  // LLVM-style target triple
};

class PlatformGenerator {
public:
    explicit PlatformGenerator(const PlatformTarget& target);
    
    // Generate platform-specific code
    void generate(std::shared_ptr<IRModule> module);
    
    // Get platform-specific prologue/epilogue
    std::string getPrologue() const;
    std::string getEpilogue() const;
    
    // Get platform-specific library linking
    std::string getLibraryLinkCommand(const std::string& libName) const;
    
    // Get system call conventions
    std::string getSyscallInstruction() const;
    
    // Platform validation
    bool isPlatformSupported(const PlatformTarget& target) const;
    
    // Get default output extension
    std::string getOutputExtension() const;
    
private:
    PlatformTarget target_;
    
    // Platform-specific helpers
    std::string getWindowsPrologue() const;
    std::string getLinuxPrologue() const;
    std::string getMacOSPrologue() const;
    std::string getUnixPrologue() const;
    
    std::string getWindowsEpilogue() const;
    std::string getLinuxEpilogue() const;
    std::string getMacOSEpilogue() const;
    std::string getUnixEpilogue() const;
    
    // Library linking
    std::string linkWindowsLibrary(const std::string& lib) const;
    std::string linkLinuxLibrary(const std::string& lib) const;
    std::string linkMacOSLibrary(const std::string& lib) const;
    std::string linkUnixLibrary(const std::string& lib) const;
};

// Platform utility functions
class PlatformUtils {
public:
    // Get target triple from platform and architecture
    static std::string getTargetTriple(Platform platform, Architecture arch);
    
    // Get binary format for platform
    static BinaryFormat getBinaryFormat(Platform platform);
    
    // Detect current platform
    static Platform getCurrentPlatform();
    
    // Check if cross-compilation is needed
    static bool needsCrossCompilation(const PlatformTarget& target);
    
    // Get system-specific includes
    static std::vector<std::string> getSystemIncludes(Platform platform);
};

} // namespace syclang

#endif // SYCLANG_IR_PLATFORM_GENERATOR_H
