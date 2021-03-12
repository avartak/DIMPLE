#ifndef DEFINITION_H
#define DEFINITION_H

#include <memory>
#include <Statement.h>
#include <Node.h>
#include <Identifier.h>

namespace avl {

    enum Storage_ID {

        STORAGE_UNDEFINED,
        STORAGE_EXTERNAL,
        STORAGE_INTERNAL,
        STORAGE_STATIC,
        STORAGE_LOCAL,
        STORAGE_PRIVATE

    };

    struct Definition : public Statement {

        uint16_t storage;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        Definition(uint16_t, const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);

    };

}

#endif
