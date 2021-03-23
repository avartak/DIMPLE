#include <llvm/IR/Constants.h>

#include <Literal.h>
#include <PrimitiveType.h>
#include <ArrayType.h>
#include <Globals.h>

namespace avl {

    IntLiteral::IntLiteral(uint64_t i):
        Value(VALUE_INT, std::make_shared<PrimitiveType>(TYPE_UINT64), TheBuilder.getInt64(i)),
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
        Value(VALUE_STRING, 
             std::make_shared<ArrayType>(std::make_shared<PrimitiveType>(TYPE_INT8), s.length()+1), 
             llvm::ConstantDataArray::getString(TheContext, s)),
        literal(s)
    {
    }

}
