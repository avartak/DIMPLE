/*

The Abstract Syntax Tree (AST) of DIMPLE
There are 3 basic elements at the root of the AST
- A set of representations of types and constant expressions
- A set of declarations of global variables and functions
- A set of definitions of global variables and functions

*/

#ifndef AST_H
#define AST_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <NameNode.h>
#include <Statement.h>

namespace avl {

    struct AST {
        std::map<std::string, std::shared_ptr<NameNode> > representations;
        std::map<std::string, std::shared_ptr<DefineStatement> > definitions;
    };

}

#endif
