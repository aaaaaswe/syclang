#include "syclang/ir/platform_generator.h"
#include <sstream>

namespace syclang {

PlatformGenerator::PlatformGenerator(const PlatformTarget& target)
    : target_(target) {}

void PlatformGenerator::generate(std::shared_ptr<IRModule> module) {
    // Platform-specific code generation
    // This integrates with the existing architecture-specific code generators
}

std::string PlatformGenerator::getPrologue() const {
    switch (target_.platform) {
        case Platform::WINDOWS:
            return getWindowsPrologue();
        case Platform::LINUX:
            return getLinuxPrologue();
        case Platform::MACOS:
            return getMacOSPrologue();
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return getUnixPrologue();
        default:
            return "";
    }
}

std::string PlatformGenerator::getEpilogue() const {
    switch (target_.platform) {
        case Platform::WINDOWS:
            return getWindowsEpilogue();
        case Platform::LINUX:
            return getLinuxEpilogue();
        case Platform::MACOS:
            return getMacOSEpilogue();
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return getUnixEpilogue();
        default:
            return "";
    }
}

std::string PlatformGenerator::getLibraryLinkCommand(const std::string& libName) const {
    switch (target_.platform) {
        case Platform::WINDOWS:
            return linkWindowsLibrary(libName);
        case Platform::LINUX:
            return linkLinuxLibrary(libName);
        case Platform::MACOS:
            return linkMacOSLibrary(libName);
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return linkUnixLibrary(libName);
        default:
            return "";
    }
}

std::string PlatformGenerator::getSyscallInstruction() const {
    switch (target_.platform) {
        case Platform::WINDOWS:
            return "syscall_win"; // Windows uses different mechanism
        case Platform::LINUX:
            if (target_.arch == Architecture::X64) {
                return "syscall";
            } else if (target_.arch == Architecture::ARM64) {
                return "svc #0";
            }
            break;
        case Platform::MACOS:
            if (target_.arch == Architecture::X64) {
                return "syscall";
            } else if (target_.arch == Architecture::ARM64) {
                return "svc #0x80";
            }
            break;
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return "syscall";
    }
    return "syscall";
}

bool PlatformGenerator::isPlatformSupported(const PlatformTarget& target) const {
    // Check if the combination is supported
    switch (target.platform) {
        case Platform::WINDOWS:
            return target.arch == Architecture::X64;
        case Platform::LINUX:
            return target.arch == Architecture::X64 || 
                   target.arch == Architecture::ARM64;
        case Platform::MACOS:
            return target.arch == Architecture::X64 || 
                   target.arch == Architecture::ARM64;
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return target.arch == Architecture::X64 || 
                   target.arch == Architecture::ARM64;
        default:
            return false;
    }
}

std::string PlatformGenerator::getOutputExtension() const {
    switch (target_.platform) {
        case Platform::WINDOWS:
            return ".exe";
        case Platform::LINUX:
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return ".elf";
        case Platform::MACOS:
            return "";
    }
    return "";
}

std::string PlatformGenerator::getWindowsPrologue() const {
    std::stringstream ss;
    ss << "# Windows Platform (PE Format)\n";
    ss << ".section .text\n";
    ss << ".global _main\n";
    ss << "_main:\n";
    return ss.str();
}

std::string PlatformGenerator::getLinuxPrologue() const {
    std::stringstream ss;
    ss << "# Linux Platform (ELF Format)\n";
    ss << ".section .text\n";
    ss << ".globl main\n";
    ss << "main:\n";
    return ss.str();
}

std::string PlatformGenerator::getMacOSPrologue() const {
    std::stringstream ss;
    ss << "# macOS Platform (Mach-O Format)\n";
    ss << ".section __TEXT,__text\n";
    ss << ".globl _main\n";
    ss << "_main:\n";
    return ss.str();
}

std::string PlatformGenerator::getUnixPrologue() const {
    std::stringstream ss;
    ss << "# Unix Platform (ELF Format)\n";
    ss << ".section .text\n";
    ss << ".globl main\n";
    ss << "main:\n";
    return ss.str();
}

std::string PlatformGenerator::getWindowsEpilogue() const {
    return "    ret\n";
}

std::string PlatformGenerator::getLinuxEpilogue() const {
    return "    ret\n";
}

std::string PlatformGenerator::getMacOSEpilogue() const {
    return "    ret\n";
}

std::string PlatformGenerator::getUnixEpilogue() const {
    return "    ret\n";
}

std::string PlatformGenerator::linkWindowsLibrary(const std::string& lib) const {
    return lib + ".lib";
}

std::string PlatformGenerator::linkLinuxLibrary(const std::string& lib) const {
    return "-l" + lib;
}

std::string PlatformGenerator::linkMacOSLibrary(const std::string& lib) const {
    return "-l" + lib;
}

std::string PlatformGenerator::linkUnixLibrary(const std::string& lib) const {
    return "-l" + lib;
}

std::string PlatformUtils::getTargetTriple(Platform platform, Architecture arch) {
    switch (platform) {
        case Platform::WINDOWS:
            if (arch == Architecture::X64) {
                return "x86_64-pc-windows-msvc";
            }
            break;
        case Platform::LINUX:
            if (arch == Architecture::X64) {
                return "x86_64-unknown-linux-gnu";
            } else if (arch == Architecture::ARM64) {
                return "aarch64-unknown-linux-gnu";
            }
            break;
        case Platform::MACOS:
            if (arch == Architecture::X64) {
                return "x86_64-apple-darwin";
            } else if (arch == Architecture::ARM64) {
                return "arm64-apple-darwin";
            }
            break;
        case Platform::UNIX:
        case Platform::FREEBSD:
            if (arch == Architecture::X64) {
                return "x86_64-unknown-freebsd";
            } else if (arch == Architecture::ARM64) {
                return "aarch64-unknown-freebsd";
            }
            break;
        case Platform::OPENBSD:
            if (arch == Architecture::X64) {
                return "x86_64-unknown-openbsd";
            }
            break;
        case Platform::NETBSD:
            if (arch == Architecture::X64) {
                return "x86_64-unknown-netbsd";
            }
            break;
    }
    return "unknown";
}

BinaryFormat PlatformUtils::getBinaryFormat(Platform platform) {
    switch (platform) {
        case Platform::WINDOWS:
            return BinaryFormat::PE;
        case Platform::LINUX:
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            return BinaryFormat::ELF;
        case Platform::MACOS:
            return BinaryFormat::MACH_O;
    }
    return BinaryFormat::ELF;
}

Platform PlatformUtils::getCurrentPlatform() {
#if defined(_WIN32) || defined(_WIN64)
    return Platform::WINDOWS;
#elif defined(__APPLE__)
    return Platform::MACOS;
#elif defined(__linux__)
    return Platform::LINUX;
#elif defined(__FreeBSD__)
    return Platform::FREEBSD;
#elif defined(__OpenBSD__)
    return Platform::OPENBSD;
#elif defined(__NetBSD__)
    return Platform::NETBSD;
#else
    return Platform::UNIX;
#endif
}

bool PlatformUtils::needsCrossCompilation(const PlatformTarget& target) {
    return getCurrentPlatform() != target.platform;
}

std::vector<std::string> PlatformUtils::getSystemIncludes(Platform platform) {
    std::vector<std::string> includes;
    
    switch (platform) {
        case Platform::WINDOWS:
            includes = {
                "C:/Program Files/Microsoft Visual Studio/VC/include",
                "C:/Program Files (x86)/Windows Kits/10/include/10.0.19041.0/ucrt"
            };
            break;
        case Platform::LINUX:
            includes = {
                "/usr/include",
                "/usr/local/include"
            };
            break;
        case Platform::MACOS:
            includes = {
                "/usr/include",
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include"
            };
            break;
        case Platform::UNIX:
        case Platform::FREEBSD:
        case Platform::OPENBSD:
        case Platform::NETBSD:
            includes = {
                "/usr/include",
                "/usr/local/include"
            };
            break;
    }
    
    return includes;
}

} // namespace syclang
