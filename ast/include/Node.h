/*

Node is the base class of all the syntactic units that are parsed by the Parser

*/

#ifndef NODE_H
#define NODE_H

#include <Location.h>

namespace avl {

    // Basic types of nodes
    // Some of the nodes branch out into further sub-types
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

    // Every Node has information about its position in the source file (loc : Location) 
    // and a type identifier (kind : uint16_t)
    // The Location member (loc) carries information about the name of the source file,
    // the start line/column of the Node and the end line/column of the Node
    struct Node {

        Location loc;
        uint16_t kind;

        Node(uint16_t);

        virtual ~Node() = default;

    };

}

#endif
