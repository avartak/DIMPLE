#ifndef ARRAYTYPENODE_H
#define ARRAYTYPENODE_H

#include <memory>
#include <TypeNode.h>

namespace avl {

    struct ArrayTypeNode : public TypeNode {

        std::shared_ptr<Node> nelements;
        std::shared_ptr<Node> array_of;

        ArrayTypeNode(const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
        ArrayTypeNode(const std::string&);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };

}

#endif
