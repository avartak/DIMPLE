#include <UnionTypeNode.h>

namespace avl {

    UnionTypeNode::UnionTypeNode(const std::shared_ptr<NameNodeSet>& m):
        TypeNode(TYPE_UNION),
        members(m)
    {
        complete = true;
    }

    UnionTypeNode::UnionTypeNode(const std::string& n):
        TypeNode(TYPE_UNION, n)
    {
    }

    bool UnionTypeNode::hasNamedType(const std::string& n) const {
        for (const auto& m : members->set) {
            if (m.node->kind == NODE_IDENTIFIER) {
                auto ident = static_cast<Identifier*>(m.node.get());
                if (ident->name == n) {
                    return true;
                }
            }
            else if (m.node->kind == NODE_TYPENODE) {
                auto type = static_cast<TypeNode*>(m.node.get());
                if (type->name == n || type->hasNamedType(n)) {
                    return true;
                }
            }
        }
        return false;
    }

    bool UnionTypeNode::construct(const std::shared_ptr<TypeNode>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto ut = static_cast<const UnionTypeNode*>(t.get());
        members = ut->members;
        complete = true;
        return true;
    }

}
