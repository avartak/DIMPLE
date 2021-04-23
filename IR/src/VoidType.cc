#include <VoidType.h>
#include <Globals.h>

namespace avl {

    VoidType::VoidType():
        Type(TYPE_VOID)
    {
        llvm_type = TheBuilder.getVoidTy();
        complete = true;
    }

    bool VoidType::operator==(const Type& t) const { 
        return is == t.is;
    }

    bool VoidType::construct(const std::shared_ptr<Type>&) {
        return false;
    }

    std::shared_ptr<Type> VoidType::clone() const {
        auto t = std::make_shared<VoidType>();
        t->attr = attr;
        t->name = name;
        t->complete = complete;
        return t;
    }

}
