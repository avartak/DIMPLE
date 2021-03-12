#include <StructTypeNode.h>

namespace avl {

    StructTypeNode::StructTypeNode(const std::shared_ptr<NameNodeSet>& m, bool p):
        TypeNode(TYPE_STRUCT),
        members(m)
    {
        if (p) {
            flags |= 1;
        }
        complete = true;
    }

    StructTypeNode::StructTypeNode(const std::string& n, bool p):
        TypeNode(TYPE_STRUCT, n)
    {
        if (p) {
            flags |= 1;
        }
    }

    bool StructTypeNode::isPacked() const {
        return (flags & 1) > 0;
    }

    bool StructTypeNode::hasNamedType(const std::string& n) const {
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

    bool StructTypeNode::construct(const std::shared_ptr<TypeNode>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto st = static_cast<const StructTypeNode*>(t.get());
        if (st->isPacked() != isPacked()) {
            return false;
        }
        members = st->members;
        complete = true;
        return true;
    }

}
