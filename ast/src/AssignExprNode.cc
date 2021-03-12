#include <AssignExprNode.h>

namespace avl {

    AssignExprNode::AssignExprNode(uint16_t o, const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r):
        ExprNode(EXPR_ASSIGN),
        op(o),
        lhs(l),
        rhs(r)
    {
    }
    
}
