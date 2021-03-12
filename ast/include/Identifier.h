#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <string>
#include <Node.h>
#include <Location.h>

namespace avl {

    struct Identifier : public Node {

        std::string name;

        Identifier();
        Identifier(const std::string&, const Location&);

    };

}

#endif
