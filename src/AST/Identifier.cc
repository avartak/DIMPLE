#include <AST/Identifier.h>

namespace dmp {

    Identifier::Identifier():
        Node(NODE_IDENTIFIER),
        name("")
    {
    }

    Identifier::Identifier(const std::string& n, const Location& l):
        Node(NODE_IDENTIFIER),
        name(n)
    {
        loc = l;
    }

}
