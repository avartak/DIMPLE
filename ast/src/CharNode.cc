#include <CharNode.h>
#include <TokenID.h>

namespace avl {

    CharNode::CharNode(const std::shared_ptr<Token>& t, char c):
        ExprNode(EXPR_CHAR),
        str(t->str),
        literal(c)
    {
        loc = t->loc;
    }

    std::shared_ptr<CharNode> CharNode::construct(const std::shared_ptr<Token>& token) {
        std::shared_ptr<CharNode> ret;

        if (token->is != TOKEN_CHAR) {
            return ret;
        }

        char c = token->str[1];
        std::size_t len = 4;

        if (c == '\\') {
            if (token->str[2] == '\\') {
                c = '\\';
            }
            else if (token->str[2] == '\'') {
                c = '\'';
            }
            else if (token->str[2] == '\"') {
                c = '\"';
            }
            else if (token->str[2] == 'a') {
                c = '\a';
            }
            else if (token->str[2] == 'b') {
                c = '\b';
            }
            else if (token->str[2] == 'f') {
                c = '\f';
            }
            else if (token->str[2] == 'n') {
                c = '\n';
            }
            else if (token->str[2] == 'r') {
                c = '\r';
            }
            else if (token->str[2] == 't') {
                c = '\t';
            }
            else if (token->str[2] == 'v') {
                c = '\v';
            }
            else if (token->str[2] == '0' || token->str[2] == '1' || token->str[2] == '2' || token->str[2] == '3') {
                c = char(strtol(token->str.substr(2, 3).c_str(), nullptr, 8));
                len = 6;
            }
            else if (token->str[2] == 'x') {
                c = char(strtol(token->str.substr(3, 2).c_str(), nullptr, 16));
                len = 6;
            }
        }
        else {
            len = 3;
        }

        if (token->str.length() != len) {
            return ret;
        }

        return std::make_shared<CharNode>(token, c);

    }
}
