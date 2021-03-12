#include <BoolNode.h>
#include <TokenID.h>

namespace avl {

    BoolNode::BoolNode(const std::shared_ptr<Token>& t, bool b):
        ExprNode(EXPR_BOOL),
        literal(b)
    {
        loc = t->loc;
    }

    std::shared_ptr<BoolNode> BoolNode::construct(const std::shared_ptr<Token>& token) {
        std::shared_ptr<BoolNode> ret;
        if (token->is != TOKEN_TRUE && token->is != TOKEN_FALSE) {
            return ret;
        }
        return std::make_shared<BoolNode>(token, token->is == TOKEN_TRUE);

    }
}
