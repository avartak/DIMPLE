#include <cstdlib>
#include <cerrno>
#include <LiteralNode.h>
#include <TokenID.h>

namespace dmp {

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
