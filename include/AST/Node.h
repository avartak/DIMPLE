#ifndef NODE_H
#define NODE_H

#include <IO/Location.h>

namespace dmp {

    enum Node_ID {

        NODE_TOKEN,
        NODE_IDENTIFIER,
        NODE_NAMENODE,
        NODE_NAMENODESET,
        NODE_TYPENODE,
        NODE_EXPRNODE,
        NODE_STATEMENT,
        NODE_NULLINIT,
        NODE_INITIALIZER,
        NODE_INITELEMENT,

    };

    struct Node {

        Location loc;
        uint16_t kind;

        Node(uint16_t);

        virtual ~Node() = default;

    };

}

#endif
