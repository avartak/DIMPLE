#include <cstdlib>
#include <cerrno>
#include <IntNode.h>
#include <TokenID.h>

namespace avl {

    IntNode::IntNode(const std::shared_ptr<Token>& t, uint64_t i):
        ExprNode(EXPR_INT),
        str(t->str),
        literal(i)
    {
        loc = t->loc;
    }

    std::shared_ptr<IntNode> IntNode::construct(const std::shared_ptr<Token>& token) {
        std::shared_ptr<IntNode> ret;

        if (token->is != TOKEN_INT) {
            return ret;
        }

        auto num = token->str;
        int base = 10;
        if (num.length() > 2) {
            switch (num[1]) {
                case 'b' : base =  2; break;
                case 'o' : base =  8; break;
                case 'x' : base = 16; break;
                default  : base = 10;
            }
            if (base != 10) {
                num = num.erase(0, 2);
            }
        }
        uint64_t i = strtoull(token->str.c_str(), nullptr, 0);
        if (errno == ERANGE) {
            return ret;
        }
        return std::make_shared<IntNode>(token, i);

    }
}
