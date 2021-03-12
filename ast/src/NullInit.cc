#include <NullInit.h>

namespace avl {

    NullInit::NullInit(bool z):
        Node(NODE_NULLINIT),
        zero(z)
    {
    }

}
