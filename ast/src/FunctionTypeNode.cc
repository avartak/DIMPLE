#include <FunctionTypeNode.h>

namespace avl {

    FunctionTypeNode::FunctionTypeNode(const std::shared_ptr<NameNodeSet>& a, const std::shared_ptr<Node>& r):
        TypeNode(TYPE_FUNCTION),
        args(a),
        ret(r)
    {
        complete = true;
    }

    FunctionTypeNode::FunctionTypeNode(const std::string& n):
        TypeNode(TYPE_UNION, n)
    {
    }

    bool FunctionTypeNode::hasNamedType(const std::string& n) const {
        for (const auto& a : args->set) {
            if (a.node->kind == NODE_IDENTIFIER) {
                auto ident = static_cast<Identifier*>(a.node.get());
                if (ident->name == n) {
                    return true;
                }
            }
            else if (a.node->kind == NODE_TYPENODE) {
                auto type = static_cast<TypeNode*>(a.node.get());
                if (type->name == n || type->hasNamedType(n)) {
                    return true;
                }
            }
        }

        if (ret->kind == NODE_IDENTIFIER) {
            auto ident = static_cast<Identifier*>(ret.get());
            if (ident->name == n) {
                return true;
            }
        }
        else if (ret->kind == NODE_TYPENODE) {
            auto type = static_cast<TypeNode*>(ret.get());
            if (type->name == n || type->hasNamedType(n)) {
                return true;
            }
        }

        return false;
    }

    bool FunctionTypeNode::construct(const std::shared_ptr<TypeNode>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto ft = static_cast<const FunctionTypeNode*>(t.get());
        args = ft->args;
        ret = ft->ret;
        complete = true;
        return true;
    }

}
