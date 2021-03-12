#include <Statement.h>

namespace avl {

    Statement::Statement(uint16_t t):
        Node(NODE_STATEMENT),
        is(t)
    {
    }

}
