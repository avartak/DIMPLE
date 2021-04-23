#include <UnknownType.h>
#include <Globals.h>

namespace avl {

    UnknownType::UnknownType():
        Type(TYPE_UNKNOWN)
    {
        llvm_type = TheBuilder.getInt8Ty();
        complete = true;
    }

    bool UnknownType::operator==(const Type& t) const {
        return is == t.is;
    }

    bool UnknownType::construct(const std::shared_ptr<Type>&) {
        return false;
    }

    std::shared_ptr<Type> UnknownType::clone() const {
        auto t = std::make_shared<UnknownType>();
        t->attr = attr;
        t->name = name;
        t->complete = complete;
        return t;
    }

}
