#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <vector>
#include <Node.h>
#include <InitElement.h>

namespace avl {

    struct Initializer : public Node {
    
        std::vector<InitElement> elements;

        Initializer(const std::vector<InitElement>&);

    };
}

#endif
