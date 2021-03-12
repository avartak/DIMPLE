#include <CallExprNode.h>

namespace avl {

    CallExprNode::CallExprNode(const std::shared_ptr<Node>& f, const std::vector<std::shared_ptr<Node> >& a):
        ExprNode(EXPR_CALL),
        func(f),
        args(a)
    {
    }
    
}

