#ifndef INITIALIZER_H
#define INITIALIZER_H

#include <memory>
#include <string>
#include <vector>
#include <AST/Node.h>

namespace dmp {

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

    struct Initializer : public Node {
    
        std::vector<InitElement> elements;

        Initializer(const std::vector<InitElement>&);

    };

    struct NullInit : public Node {

        bool zero;

        NullInit(bool);

    };

}

#endif
