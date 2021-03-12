#include <Initializer.h>

namespace avl {

    Initializer::Initializer(const std::vector<InitElement>& ev):
        Node(NODE_INITIALIZER),
        elements(ev)
    {
    }

}
