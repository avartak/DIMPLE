#ifndef POINTERTYPENODE_H
#define POINTERTYPENODE_H

#include <memory>
#include <TypeNode.h>

namespace avl {

    struct PointerTypeNode : public TypeNode {

        std::shared_ptr<Node> points_to;

        PointerTypeNode(const std::shared_ptr<Node>&);
        PointerTypeNode(const std::string&);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };

}

#endif
