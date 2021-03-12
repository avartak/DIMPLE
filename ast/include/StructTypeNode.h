#ifndef STRUCTTYPENODE_H
#define STRUCTTYPENODE_H

#include <memory>
#include <TypeNode.h>
#include <NameNodeSet.h>

namespace avl {

    struct StructTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> members;

        StructTypeNode(const std::shared_ptr<NameNodeSet>&, bool);
        StructTypeNode(const std::string&, bool);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;

        bool isPacked() const;
    };
  
}

#endif
