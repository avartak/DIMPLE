#include <UnaryExprNode.h>

namespace avl {

    UnaryExprNode::UnaryExprNode(uint16_t o, const std::shared_ptr<Node>& e):
        ExprNode(EXPR_UNARY),
        op(o),
        exp(e)
    {
    }
}
