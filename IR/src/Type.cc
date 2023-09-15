#include <Type.h>
#include <Globals.h>

namespace avl {

    std::vector<std::pair<const Type*, const Type*> > Type::being_compared;

    Type::Type(uint16_t t):
        Entity(ENTITY_TYPE),
        TypeBase(t),
        name(""),
        llvm_type(nullptr),
        complete(false)
    {
    }
    
    Type::Type(uint16_t t, const std::string& n):
        Entity(ENTITY_TYPE),
        TypeBase(t),
        name(n),
        llvm_type(llvm::StructType::create(*TheContext, n)),
        complete(false)
    {
    }


    bool Type::operator!=(const Type& t) const {
        return !operator==(t);
    }

    std::size_t Type::size() const {
        if (isUnknown() || isVoid() || isFunction() || !isComplete()) return 0;
        if (llvm_type == nullptr) return 0;
        return TheModule->getDataLayout().getTypeAllocSize(llvm_type).getFixedValue();
    }

    std::size_t Type::alignment() const {
        if (isFunction()) {
            return 8;
        }
        auto sz = size();
        switch (sz) {
            case 0 : return 0;
            case 1 : return 1;
            case 2 : return 2;
            case 3 : return 4;
            case 4 : return 4;
            default: return 8;
        }
    }

    bool Type::moveDirectly() const {
        return !isCompound();
    }
    
    bool Type::passDirectly() const {
        return size() <= 8;
    }
    
    bool Type::retDirectly() const {
        return size() <= 8;
    }

}
