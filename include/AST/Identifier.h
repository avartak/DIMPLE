#ifndef IDENTIFIER_H
#define IDENTIFIER_H

#include <string>
#include <Start/Location.h>
#include <AST/Node.h>

namespace dmp {

    struct Identifier : public Node {

        std::string name;

        Identifier();
        Identifier(const std::string&, const Location&);

    };

}

#endif
