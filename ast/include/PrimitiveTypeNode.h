#ifndef PRIMITIVETYPENODE_H
#define PRIMITIVETYPENODE_H

#include <TypeNode.h>

namespace avl {

    struct PrimitiveTypeNode : public TypeNode {

        PrimitiveTypeNode(uint16_t);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };

}

#endif
