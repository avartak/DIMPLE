#include <ArrayTypeNode.h>
#include <Identifier.h>

namespace avl {

    ArrayTypeNode::ArrayTypeNode(const std::shared_ptr<Node>& ao, const std::shared_ptr<Node>& ne):
        TypeNode(TYPE_ARRAY),
        array_of(ao),
        nelements(ne)
    {
        complete = true;
    }

    ArrayTypeNode::ArrayTypeNode(const std::string& n):
        TypeNode(TYPE_ARRAY, n)
    {
    }

    bool ArrayTypeNode::hasNamedType(const std::string& n) const {
        if (array_of->kind == NODE_IDENTIFIER) {
            auto ident = static_cast<Identifier*>(array_of.get());
            if (ident->name == n) {
                return true;
            }
        }
        else if (array_of->kind == NODE_TYPENODE) {
            auto type = static_cast<TypeNode*>(array_of.get());
            if (type->name == n || type->hasNamedType(n)) {
                return true;
            }
        }
        return false;
    }

    bool ArrayTypeNode::construct(const std::shared_ptr<TypeNode>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto at = static_cast<const ArrayTypeNode*>(t.get());
        array_of = at->array_of;
        nelements = at->nelements;
        complete = true;
        return true;
    }

}
