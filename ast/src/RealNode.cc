#include <cstdlib>
#include <cerrno>
#include <RealNode.h>
#include <TokenID.h>

namespace avl {

    RealNode::RealNode(const std::shared_ptr<Token>& t, double d):
        ExprNode(EXPR_REAL),
        str(t->str),
        literal(d)
    {
        loc = t->loc;
    }

    std::shared_ptr<RealNode> RealNode::construct(const std::shared_ptr<Token>& token) {
        std::shared_ptr<RealNode> ret;

        if (token->is != TOKEN_REAL) {
            return ret;
        }
        double d = strtod(token->str.c_str(), nullptr);
        if (errno == ERANGE) {
            return ret;
        }
        return std::make_shared<RealNode>(token, d);

    }
}
