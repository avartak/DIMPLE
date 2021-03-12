#include <TypeBase.h>

namespace avl {

    TypeBase::TypeBase(uint16_t t):
        is(t),
        flags(0),
        name(""),
        complete(false)
    {
    }

    TypeBase::TypeBase(uint16_t t, const std::string& n):
        is(t),
        flags(0),
        name(n),
        complete(false)
    {
    }

}
