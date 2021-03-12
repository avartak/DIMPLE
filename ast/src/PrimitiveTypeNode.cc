#include <PrimitiveTypeNode.h>

namespace avl {

    PrimitiveTypeNode::PrimitiveTypeNode(uint16_t t):
        TypeNode(t)
    {
        complete = true;
    }

    bool PrimitiveTypeNode::hasNamedType(const std::string&) const {
        return false;
    }

    bool PrimitiveTypeNode::construct(const std::shared_ptr<TypeNode>&) {
        return false;
    }
}
