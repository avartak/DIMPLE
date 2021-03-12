#include <PointerTypeNode.h>
#include <Identifier.h>

namespace avl {

    PointerTypeNode::PointerTypeNode(const std::shared_ptr<Node>& pt):
        TypeNode(TYPE_POINTER),
        points_to(pt)
    {
        complete = true;
    }

    PointerTypeNode::PointerTypeNode(const std::string& n):
        TypeNode(TYPE_POINTER, n)
    {
    }

    bool PointerTypeNode::hasNamedType(const std::string& n) const {
        if (points_to->kind == NODE_IDENTIFIER) {
            auto ident = static_cast<Identifier*>(points_to.get());
            if (ident->name == n) {
                return true;
            }
        }
        else if (points_to->kind == NODE_TYPENODE) {
            auto type = static_cast<TypeNode*>(points_to.get());
            if (type->name == n || type->hasNamedType(n)) {
                return true;
            }
        }
        return false;
    }

    bool PointerTypeNode::construct(const std::shared_ptr<TypeNode>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto pt = static_cast<const PointerTypeNode*>(t.get());
        points_to = pt->points_to;
        complete = true;
        return true;
    }

}
