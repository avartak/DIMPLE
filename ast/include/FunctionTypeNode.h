#ifndef FUNCTIONTYPENODE_H
#define FUNCTIONTYPENODE_H

#include <memory>
#include <TypeNode.h>
#include <NameNodeSet.h>

namespace avl {

    struct FunctionTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> args;
        std::shared_ptr<Node> ret;

        FunctionTypeNode(const std::shared_ptr<NameNodeSet>&, const std::shared_ptr<Node>&);

        virtual bool hasNamedType(const std::string&) const override;
        virtual bool construct(const std::shared_ptr<TypeNode>&) override;
    };
 
}

#endif
