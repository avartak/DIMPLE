#ifndef UNKNOWNTYPENODE_H
#define UNKNOWNTYPENODE_H

#include <TypeNode.h>

namespace avl {

    struct UnknownTypeNode : public TypeNode {

        UnknownTypeNode();

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };

 
}

#endif
