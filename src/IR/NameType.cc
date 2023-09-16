#include <IR/NameType.h>

namespace dmp {

    NameType::NameType():
        attr(0)
    {
    }
    
    NameType::NameType(const std::shared_ptr<Type>& ty):
        type(ty),
        attr(0)
    {
    }
    
    NameType::NameType(const std::shared_ptr<Type>& ty, uint64_t a):
        type(ty),
        attr(a)
    {
    }
    
    NameType::NameType(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Type>& ty):
        name(nm),
        type(ty),
        attr(0)
    {
    }

    NameType::NameType(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Type>& ty, uint64_t a):
        name(nm),
        type(ty),
        attr(a)
    {
    }

}
