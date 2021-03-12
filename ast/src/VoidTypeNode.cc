#include <VoidTypeNode.h>

namespace avl {

    VoidTypeNode::VoidTypeNode():
        TypeNode(TYPE_VOID)
    {
        complete = true;
    }

    bool VoidTypeNode::hasNamedType(const std::string&) const {
        return false;
    }

    bool VoidTypeNode::construct(const std::shared_ptr<TypeNode>&) {
        return false;
    }

}
