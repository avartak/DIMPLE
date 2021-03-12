#include <UnknownTypeNode.h>

namespace avl {

    UnknownTypeNode::UnknownTypeNode():
        TypeNode(TYPE_UNKNOWN)
    {
        complete = true;
    }

    bool UnknownTypeNode::hasNamedType(const std::string&) const {
        return false;
    }

    bool UnknownTypeNode::construct(const std::shared_ptr<TypeNode>&) {
        return false;
    }
}
