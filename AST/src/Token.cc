#include <Token.h>
#include <TokenID.h>

namespace dmp {

    Token::Token():
        Node(NODE_TOKEN),
        is(TOKEN_NONE),
        str("")
    {
    }

    Token::Token(int t, const std::string& s, const Location& l):
        Node(NODE_TOKEN),
        is(t),
        str(s)
    {
        loc = l;
    }
    
}

