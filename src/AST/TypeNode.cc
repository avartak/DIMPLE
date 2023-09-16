#include <AST/TypeNode.h>

namespace dmp {

    TypeNode::TypeNode(uint16_t t):
        Node(NODE_TYPENODE),
        TypeBase(t)
    {
    }

    UnknownTypeNode::UnknownTypeNode():
        TypeNode(TYPE_UNKNOWN)
    {
    }

    VoidTypeNode::VoidTypeNode():
        TypeNode(TYPE_VOID)
    {
    }

    PrimitiveTypeNode::PrimitiveTypeNode(uint16_t t):
        TypeNode(t)
    {
    }

    PointerTypeNode::PointerTypeNode(const std::shared_ptr<Node>& pt):
        TypeNode(TYPE_POINTER),
        points_to(pt)
    {
    }

    ArrayTypeNode::ArrayTypeNode(const std::shared_ptr<Node>& ao, const std::shared_ptr<Node>& ne):
        TypeNode(TYPE_ARRAY),
        array_of(ao),
        nelements(ne)
    {
    }

    StructTypeNode::StructTypeNode(const std::shared_ptr<NameNodeSet>& m, bool p):
        TypeNode(TYPE_STRUCT),
        members(m)
    {
        if (p) {
            attr |= 1;
        }
    }

    bool StructTypeNode::isPacked() const {
        return (attr & 1) > 0;
    }

    UnionTypeNode::UnionTypeNode(const std::shared_ptr<NameNodeSet>& m):
        TypeNode(TYPE_UNION),
        members(m)
    {
    }

    FunctionTypeNode::FunctionTypeNode(const std::shared_ptr<NameNodeSet>& a, const std::shared_ptr<Node>& r):
        TypeNode(TYPE_FUNCTION),
        args(a),
        ret(r)
    {
    }

}
