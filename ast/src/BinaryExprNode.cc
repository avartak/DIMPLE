#include <BinaryExprNode.h>

namespace avl {

    BinaryExprNode::BinaryExprNode(uint16_t o, const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r):
        ExprNode(EXPR_BINARY),
        op(o),
        lhs(l),
        rhs(r)
    {
    }

}
