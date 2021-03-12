#include <TypeNode.h>

namespace avl {

    TypeNode::TypeNode(uint16_t t):
        Node(NODE_TYPENODE),
        TypeBase(t)
    {
    }

    TypeNode::TypeNode(uint16_t t, const std::string& n):
        Node(NODE_TYPENODE),
        TypeBase(t, n)
    {
    }


}
