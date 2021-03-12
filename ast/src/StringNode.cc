#include <StringNode.h>
#include <TokenID.h>

namespace avl {

    StringNode::StringNode(const std::shared_ptr<Token>& t, const std::string& l):
        ExprNode(EXPR_STRING),
        str(t->str),
        literal(l)
    {
        loc = t->loc;
    }

    std::shared_ptr<StringNode> StringNode::construct(const std::shared_ptr<Token>& token) {
        std::shared_ptr<StringNode> ret;

        if (token->is != TOKEN_STRING) {
            return ret;
        }

        std::size_t pos = 1;
        std::string s = "";

        while (pos < token->str.length()-1) {
            if (token->str[pos] == '\\') {
                pos++;
                if (token->str[pos] == '\'') {
                    s += '\'';
                    pos++;
                }
                else if (token->str[pos] == '\"') {
                    s += '\"';
                    pos++;
                }
                else if (token->str[pos] == '\?') {
                    s += '\?';
                    pos++;
                }
                else if (token->str[pos] == 'a') {
                    s += '\a';
                    pos++;
                }
                else if (token->str[pos] == 'b') {
                    s += '\b';
                    pos++;
                }
                else if (token->str[pos] == 'f') {
                    s += '\f';
                    pos++;
                }
                else if (token->str[pos] == 'n') {
                    s += '\n';
                    pos++;
                }
                else if (token->str[pos] == 'r') {
                    s += '\r';
                    pos++;
                }
                else if (token->str[pos] == 't') {
                    s += '\t';
                    pos++;
                }
                else if (token->str[pos] == 'v') {
                    s += '\v';
                    pos++;
                }
                else if (token->str[pos] == '0' || token->str[pos] == '1' || token->str[pos] == '2' || token->str[pos] == '3') {
                    s += char(strtol(token->str.substr(pos, 3).c_str(), nullptr, 8));
                    pos += 3;
                }
                else if (token->str[pos] == 'x') {
                    s += char(strtol(token->str.substr(pos+1, 2).c_str(), nullptr, 8));
                    pos += 3;
                }
            }
            else {
                s += token->str[pos];
                pos++;
            }
        }

        return std::make_shared<StringNode>(token, s);

    }
}
