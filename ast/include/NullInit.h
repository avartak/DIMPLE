#ifndef NULLINIT_H
#define NULLINIT_H

#include <Node.h>

namespace avl {

    struct NullInit : public Node {
   
        bool zero;
        
        NullInit(bool);

    };
    
}

#endif
