#include <Initializer.h>

namespace avl {

    InitElement::InitElement(const std::shared_ptr<Node>& v):
        Node(NODE_INITELEMENT),
        is(INIT_UNTAGGED),
        tag(),
        value(v)
    {
        loc = v->loc;
    }

    InitElement::InitElement(uint16_t tt, const std::shared_ptr<Node>& t, const std::shared_ptr<Node>& v):
        Node(NODE_INITELEMENT),
        is(tt),
        tag(t),
        value(v)
    {
        loc = t->loc;
        loc.end = v->loc.end;
    }

    Initializer::Initializer(const std::vector<InitElement>& ev):
        Node(NODE_INITIALIZER),
        elements(ev)
    {
    }

    NullInit::NullInit(bool z):
        Node(NODE_NULLINIT),
        zero(z)
    {
    }

}
