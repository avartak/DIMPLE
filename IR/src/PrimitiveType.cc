#include <PrimitiveType.h>
#include <Globals.h>

namespace avl {

    PrimitiveType::PrimitiveType(int t):
        Type(t)
    {
        if (t == TYPE_BOOL) {
            llvm_type = TheBuilder->getInt1Ty();
        }
        else if (t == TYPE_UINT8  || t == TYPE_INT8 ) {
            llvm_type = TheBuilder->getInt8Ty();
        }
        else if (t == TYPE_UINT16 || t == TYPE_INT16) {
            llvm_type = TheBuilder->getInt16Ty();
        }
        else if (t == TYPE_UINT32 || t == TYPE_INT32) {
            llvm_type = TheBuilder->getInt32Ty();
        }
        else if (t == TYPE_UINT64 || t == TYPE_INT64) {
            llvm_type = TheBuilder->getInt64Ty();
        }
        else if (t == TYPE_REAL32) {
            llvm_type = TheBuilder->getFloatTy();
        }
        else if (t == TYPE_REAL64) {
            llvm_type = TheBuilder->getDoubleTy();
        }
        else {
            llvm_type = nullptr; // This should never happen
        }
        complete = true;
    }

    bool PrimitiveType::operator==(const Type& t) const {
        return is == t.is;
    }
  
    bool PrimitiveType::construct(const std::shared_ptr<Type>&) {
        return false;
    }

    std::shared_ptr<Type> PrimitiveType::clone() const {
        auto t = std::make_shared<PrimitiveType>(is);
        t->attr = attr;
        t->name = name;
        t->complete = complete;
        return t;
    }

}
