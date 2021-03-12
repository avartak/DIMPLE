#ifndef VOIDTYPENODE_H
#define VOIDTYPENODE_H

#include <TypeNode.h>

namespace avl {

    struct VoidTypeNode : public TypeNode {

        VoidTypeNode();

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };
 
}

#endif
