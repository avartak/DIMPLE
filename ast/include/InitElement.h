#ifndef INITELEMENT_H
#define INITELEMENT_H

#include <string>
#include <memory>
#include <Node.h>

namespace avl {

    enum Init_ID {

        INIT_UNTAGGED,
        INIT_LABELED,
        INIT_INDEXED

    };

    struct InitElement : public Node {
    
        uint16_t is;
        std::shared_ptr<Node> tag;
        std::shared_ptr<Node> value;

        InitElement(const std::shared_ptr<Node>&);
        InitElement(uint16_t, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
    
    };
    
}

#endif
