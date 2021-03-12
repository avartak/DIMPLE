#include <Definition.h>

namespace avl {

    Definition::Definition(uint16_t s, const std::shared_ptr<Identifier>& n, const std::shared_ptr<Node>& t, const std::shared_ptr<Node>& d):
        Statement(STATEMENT_DEFINITION),
        storage(s),
        name(n),
        type(t),
        def(d)
    {
        loc = name->loc;
        loc.end = def->loc.end;
    }

}
