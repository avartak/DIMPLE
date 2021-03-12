#ifndef UNIONTYPENODE_H
#define UNIONTYPENODE_H

#include <memory>
#include <TypeNode.h>
#include <NameNodeSet.h>

namespace avl {

    struct UnionTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> members;

        UnionTypeNode(const std::shared_ptr<NameNodeSet>&);
        UnionTypeNode(const std::string&);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };
  
}

#endif
