#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

#include <Literal.h>
#include <PrimitiveType.h>
#include <PointerType.h>
#include <ArrayType.h>
#include <Globals.h>

namespace avl {

    IntLiteral::IntLiteral(uint64_t i):
        Value(VALUE_INT, std::make_shared<PrimitiveType>(TYPE_INT64), TheBuilder.getInt64(i)),
        literal(i)
    {
    }

    BoolLiteral::BoolLiteral(bool b):
        Value(VALUE_BOOL, std::make_shared<PrimitiveType>(TYPE_BOOL), TheBuilder.getInt1(b)),
        literal(b)
    {
    }

    RealLiteral::RealLiteral(double d):
        Value(VALUE_REAL, std::make_shared<PrimitiveType>(TYPE_REAL64), llvm::ConstantFP::get(TheBuilder.getDoubleTy(), llvm::APFloat(d))),
        literal(d)
    {
    }

    CharLiteral::CharLiteral(char c):
        Value(VALUE_CHAR, std::make_shared<PrimitiveType>(TYPE_INT8), TheBuilder.getInt8(c)),
        literal(c)
    {
    }

    StringLiteral::StringLiteral(const std::string& s):
        Value(VALUE_STRING, std::make_shared<PointerType>(std::make_shared<PrimitiveType>(TYPE_INT8)), nullptr),
        literal(s)
    {
        auto str_type = std::make_shared<ArrayType>(std::make_shared<PrimitiveType>(TYPE_INT8), s.length()+1);
        auto str_const = llvm::ConstantDataArray::getString(TheContext, s);
        auto arr = new llvm::GlobalVariable(*TheModule, str_type->llvm_type, false, llvm::GlobalValue::PrivateLinkage, str_const, "");
        arr->setAlignment(llvm::Align(str_type->alignment()));
        llvm_value = TheBuilder.CreatePointerCast(arr, type->llvm_type);
    }

}
