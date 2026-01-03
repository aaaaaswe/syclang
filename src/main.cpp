#include <iostream>
#include <fstream>
#include <string>
#include "syclang/lexer/lexer.h"
#include "syclang/parser/parser.h"
#include "syclang/ir/ir_generator.h"
#include "syclang/codegen/x64/x64_codegen.h"
#include "syclang/codegen/arm64/arm64_codegen.h"
#include "syclang/ir/ir.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] <input_file>\n"
              << "\nOptions:\n"
              << "  --arch <architecture>  Target architecture (x64 or arm64, default: x64)\n"
              << "  --output <file>       Output file (default: output.s)\n"
              << "  --format <format>     Output format (elf, pe, efi, raw, default: elf)\n"
              << "  --ir                  Output IR instead of assembly\n"
              << "  --help                Show this help message\n"
              << "\nExample:\n"
              << "  " << programName << " --arch x64 --output program.s hello.syl\n"
              << "  " << programName << " --arch arm64 --format efi --output boot.efi efi_hello.syl\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file '" << filename << "'\n";
        exit(1);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot create file '" << filename << "'\n";
        exit(1);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parse command line arguments
    std::string inputFile;
    std::string outputFile = "output.s";
    Architecture arch = Architecture::X64;
    OutputFormat format = OutputFormat::ELF;
    bool outputIR = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--arch" && i + 1 < argc) {
            std::string archStr = argv[++i];
            if (archStr == "x64" || archStr == "x86_64") {
                arch = Architecture::X64;
            } else if (archStr == "arm64" || archStr == "aarch64") {
                arch = Architecture::ARM64;
            } else {
                std::cerr << "Error: Unknown architecture '" << archStr << "'\n";
                return 1;
            }
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--format" && i + 1 < argc) {
            std::string formatStr = argv[++i];
            if (formatStr == "elf") {
                format = OutputFormat::ELF;
            } else if (formatStr == "pe") {
                format = OutputFormat::PE;
            } else if (formatStr == "efi") {
                format = OutputFormat::EFI;
            } else if (formatStr == "raw") {
                format = OutputFormat::RAW;
            } else {
                std::cerr << "Error: Unknown format '" << formatStr << "'\n";
                return 1;
            }
        } else if (arg == "--ir") {
            outputIR = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "SysLang Compiler v1.0.0\n";
    std::cout << "======================\n";
    
    // Read source file
    std::cout << "Reading source file: " << inputFile << "\n";
    std::string source = readFile(inputFile);
    
    // Lexical analysis
    std::cout << "Lexical analysis...\n";
    syclang::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    std::cout << "  Found " << tokens.size() << " tokens\n";
    
    // Syntax parsing
    std::cout << "Parsing...\n";
    syclang::Parser parser(tokens);
    auto program = parser.parse();
    
    if (!parser.getErrors().empty()) {
        std::cerr << "\nParsing errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    std::cout << "  Parsed " << program->declarations.size() << " declarations\n";
    
    // IR generation
    std::cout << "Generating IR...\n";
    syclang::IRGenerator irGenerator(arch);
    auto module = irGenerator.generate(program);
    module->outputFormat = format;
    std::cout << "  Generated " << module->functions.size() << " functions\n";
    
    // Output IR or assembly
    std::string output;
    if (outputIR) {
        std::cout << "Outputting IR...\n";
        output = module->dump();
    } else {
        std::cout << "Code generation for " 
                  << (arch == Architecture::X64 ? "x64" : "ARM64") << "...\n";
        
        std::unique_ptr<syclang::CodeGenerator> codegen;
        if (arch == Architecture::X64) {
            codegen = std::make_unique<syclang::X64CodeGenerator>();
        } else {
            codegen = std::make_unique<syclang::ARM64CodeGenerator>();
        }
        
        codegen->generate(module);
        output = codegen->getOutput();
    }
    
    // Write output file
    writeFile(outputFile, output);
    std::cout << "Output written to: " << outputFile << "\n";
    
    // Post-processing instructions
    if (format == OutputFormat::EFI) {
        std::cout << "\nNote: For EFI application, you need to:\n";
        std::cout << "  1. Assemble the output with: as -o output.o " << outputFile << "\n";
        std::cout << "  2. Link with EFI libraries: ld -o " << outputFile.substr(0, outputFile.find_last_of('.')) << ".efi output.o -lefi\n";
    } else {
        std::cout << "\nTo assemble and link:\n";
        std::cout << "  as -o output.o " << outputFile << "\n";
        std::cout << "  ld -o program output.o\n";
    }
    
    std::cout << "\nCompilation successful!\n";
    return 0;
}
