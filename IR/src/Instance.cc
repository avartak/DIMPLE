#include <Instance.h>

namespace avl {

    Instance::Instance(uint16_t id, uint16_t s, const std::string& n, const std::shared_ptr<Type>& t):
        Value(id, t, nullptr),
        storage(s),
        name(n)
    {
    }

}
