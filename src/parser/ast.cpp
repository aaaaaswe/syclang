#include "syclang/parser/ast.h"
#include <sstream>

namespace syclang {

size_t Type::getSize() const {
    switch (category) {
        case TypeCategory::I8:
        case TypeCategory::U8:
        case TypeCategory::BOOL:
            return 1;
        case TypeCategory::I16:
        case TypeCategory::U16:
            return 2;
        case TypeCategory::I32:
        case TypeCategory::U32:
        case TypeCategory::F32:
            return 4;
        case TypeCategory::I64:
        case TypeCategory::U64:
        case TypeCategory::F64:
        case TypeCategory::POINTER:
            return 8;
        case TypeCategory::VOID:
            return 0;
        case TypeCategory::ARRAY:
            return baseType->getSize() * arraySize;
        case TypeCategory::STRUCT: {
            // This would need actual struct definition
            return 8; // Placeholder
        }
        case TypeCategory::ENUM:
            return 4; // Assuming 32-bit enum
        case TypeCategory::FUNCTION:
            return 8; // Function pointer size
    }
    return 0;
}

std::string Type::toString() const {
    switch (category) {
        case TypeCategory::I8: return "i8";
        case TypeCategory::I16: return "i16";
        case TypeCategory::I32: return "i32";
        case TypeCategory::I64: return "i64";
        case TypeCategory::U8: return "u8";
        case TypeCategory::U16: return "u16";
        case TypeCategory::U32: return "u32";
        case TypeCategory::U64: return "u64";
        case TypeCategory::F32: return "f32";
        case TypeCategory::F64: return "f64";
        case TypeCategory::BOOL: return "bool";
        case TypeCategory::VOID: return "void";
        case TypeCategory::POINTER: return "*" + baseType->toString();
        case TypeCategory::ARRAY: {
            std::stringstream ss;
            ss << "[" << arraySize << "]" << baseType->toString();
            return ss.str();
        }
        case TypeCategory::STRUCT: return "struct " + name;
        case TypeCategory::ENUM: return "enum " + name;
        case TypeCategory::FUNCTION: return "fn";
    }
    return "unknown";
}

// Expression accept methods
void LiteralExpr::accept(ASTVisitor& visitor) { visitor.visitLiteral(this); }
void IdentifierExpr::accept(ASTVisitor& visitor) { visitor.visitIdentifier(this); }
void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visitBinary(this); }
void UnaryExpr::accept(ASTVisitor& visitor) { visitor.visitUnary(this); }
void CallExpr::accept(ASTVisitor& visitor) { visitor.visitCall(this); }
void CastExpr::accept(ASTVisitor& visitor) { visitor.visitCast(this); }
void IndexExpr::accept(ASTVisitor& visitor) { visitor.visitIndex(this); }
void MemberAccessExpr::accept(ASTVisitor& visitor) { visitor.visitMemberAccess(this); }
void AsmExpr::accept(ASTVisitor& visitor) { visitor.visitAsm(this); }

// Statement accept methods
void BlockStmt::accept(ASTVisitor& visitor) { visitor.visitBlock(this); }
void LetStmt::accept(ASTVisitor& visitor) { visitor.visitLet(this); }
void ExprStmt::accept(ASTVisitor& visitor) { visitor.visitExpr(this); }
void ReturnStmt::accept(ASTVisitor& visitor) { visitor.visitReturn(this); }
void IfStmt::accept(ASTVisitor& visitor) { visitor.visitIf(this); }
void WhileStmt::accept(ASTVisitor& visitor) { visitor.visitWhile(this); }
void ForStmt::accept(ASTVisitor& visitor) { visitor.visitFor(this); }

// Declaration accept methods
void FunctionDecl::accept(ASTVisitor& visitor) { visitor.visitFunction(this); }
void StructDecl::accept(ASTVisitor& visitor) { visitor.visitStruct(this); }
void EnumDecl::accept(ASTVisitor& visitor) { visitor.visitEnum(this); }

} // namespace syclang
