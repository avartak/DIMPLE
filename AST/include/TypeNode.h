#ifndef TYPENODE_H
#define TYPENODE_H

#include <memory>
#include <string>
#include <Node.h>
#include <TypeBase.h>
#include <NameNode.h>

namespace dmp {

    struct TypeNode : public Node, public TypeBase {

        TypeNode(uint16_t);

    };

    struct UnknownTypeNode : public TypeNode {

        UnknownTypeNode();

    };

    struct VoidTypeNode : public TypeNode {

        VoidTypeNode();

    };

    struct PrimitiveTypeNode : public TypeNode {

        PrimitiveTypeNode(uint16_t);

    };

    struct PointerTypeNode : public TypeNode {

        std::shared_ptr<Node> points_to;

        PointerTypeNode(const std::shared_ptr<Node>&);
    };

    struct ArrayTypeNode : public TypeNode {

        std::shared_ptr<Node> nelements;
        std::shared_ptr<Node> array_of;

        ArrayTypeNode(const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
    };

    struct StructTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> members;

        StructTypeNode(const std::shared_ptr<NameNodeSet>&, bool);

        bool isPacked() const;
    };

    struct UnionTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> members;

        UnionTypeNode(const std::shared_ptr<NameNodeSet>&);
    };

    struct FunctionTypeNode : public TypeNode {

        std::shared_ptr<NameNodeSet> args;
        std::shared_ptr<Node> ret;

        FunctionTypeNode(const std::shared_ptr<NameNodeSet>&, const std::shared_ptr<Node>&);
    };

}

#endif
