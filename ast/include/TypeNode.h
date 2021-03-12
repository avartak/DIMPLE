#ifndef TYPENODE_H
#define TYPENODE_H

#include <memory>
#include <string>
#include <Node.h>
#include <TypeBase.h>

namespace avl {

    struct TypeNode : public Node, public TypeBase {

        TypeNode(uint16_t);
        TypeNode(uint16_t, const std::string&);

        virtual bool hasNamedType(const std::string&) const = 0;
        virtual bool construct(const std::shared_ptr<TypeNode>&) = 0;
    };

}

#endif
