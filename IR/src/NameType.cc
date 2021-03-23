#include <NameType.h>

namespace avl {

    NameType::NameType() {
    }
    
    NameType::NameType(const std::shared_ptr<Type>& ty):
        type(ty)
    {
    }
    
    NameType::NameType(const std::shared_ptr<Identifier>& nm, const std::shared_ptr<Type>& ty):
        name(nm),
        type(ty)
    {
    }

}
