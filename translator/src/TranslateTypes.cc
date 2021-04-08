#include <sstream>
#include <Translator.h>
#include <UnknownType.h>
#include <VoidType.h>
#include <PrimitiveType.h>
#include <PointerType.h>
#include <ArrayType.h>
#include <StructType.h>
#include <UnionType.h>
#include <FunctionType.h>
#include <Value.h>

namespace avl {

    bool Translator::getType(const std::shared_ptr<Node>& node, bool includeOpaquePtr) {

        if (node->kind == NODE_IDENTIFIER) {
            return getTypeRep(std::static_pointer_cast<Identifier>(node), includeOpaquePtr);
        }
        if (node->kind != NODE_TYPENODE) {
            return error();
        }
        auto tnode = std::static_pointer_cast<TypeNode>(node);

        if (tnode->isPrimitive()) {
            result = std::make_shared<PrimitiveType>(tnode->is);
            return success();
        }
        switch (tnode->is) {
            case TYPE_UNKNOWN  : result = std::make_shared<UnknownType>(); return success();
            case TYPE_VOID     : result = std::make_shared<VoidType>(); return success();
            case TYPE_POINTER  : return getPtrType(std::static_pointer_cast<PointerTypeNode>(tnode), includeOpaquePtr);
            case TYPE_ARRAY    : return getArrayType(std::static_pointer_cast<ArrayTypeNode>(tnode), includeOpaquePtr);
            case TYPE_STRUCT   : return getStructType(std::static_pointer_cast<StructTypeNode>(tnode), includeOpaquePtr);
            case TYPE_UNION    : return getUnionType(std::static_pointer_cast<UnionTypeNode>(tnode), includeOpaquePtr);
            case TYPE_FUNCTION : return getFunctionType(std::static_pointer_cast<FunctionTypeNode>(tnode), includeOpaquePtr);
            default            : return error(tnode, "Unable to decipher type"); 
        }
    }

    bool Translator::getTypeRep(const std::shared_ptr<Identifier>& ident, bool includeOpaquePtr) {

        const auto& n = ident->name;

        if (gst->types.find(n) != gst->types.end()) {
            if (gst->types[n]->isComplete() || includeOpaquePtr) {
                result = gst->types[n];
                return success();
            }
            else {
                return error(ident, "Type " + n + " is incomplete");
            }
        }

        if (ast->representations.find(n) == ast->representations.end()) {
            return error();
        }
        auto nsnode = ast->getNonSynonymRepNode(ident);
        if (nsnode->kind != NODE_TYPENODE) {
            return error();
        }
        auto tnode = std::static_pointer_cast<TypeNode>(nsnode);

        if (tnode->isPrimitive()) {
            gst->types[n] = std::make_shared<PrimitiveType>(tnode->is);
        }
        else {
            switch (tnode->is) {
                case TYPE_POINTER  : gst->types[n] = std::make_shared<PointerType>(n); break;
                case TYPE_ARRAY    : gst->types[n] = std::make_shared<ArrayType>(n); break;
                case TYPE_STRUCT   : gst->types[n] = std::make_shared<StructType>(n, static_cast<StructTypeNode*>(tnode.get())->isPacked()); break;
                case TYPE_UNION    : gst->types[n] = std::make_shared<UnionType>(n); break;
                case TYPE_FUNCTION : gst->types[n+".ptr"] = std::make_shared<PointerType>(n); break;
                default            : return error(tnode, "Unable to decipher type");
            }
        }

        if (!getType(ast->representations[n]->node, includeOpaquePtr)) {
            return error();
        }
        auto ty = std::static_pointer_cast<Type>(result);
        if (ty->isFunction()) {
            gst->types[n] = ty;
            gst->types[n+".ptr"]->construct(std::make_shared<PointerType>(gst->types[n]));
        }
        else {
            gst->types[n]->construct(ty);
        }

        result = gst->types[n];
        return success();
    }

    bool Translator::getPtrType(const std::shared_ptr<PointerTypeNode>& tnode, bool includeOpaquePtr) {

        auto ptnode = tnode->points_to;
        if (ptnode->kind == NODE_IDENTIFIER) {
            auto ident = std::static_pointer_cast<Identifier>(ptnode);
            const auto& n = ident->name;
            if (gst->types.find(n+".ptr") != gst->types.end()) {
                result = gst->types[n+".ptr"];
                return success();
            }
        }
        if (!getType(ptnode, true)) {
            return error(ptnode, "Unable to create pointee type");
        }
        auto points_to = std::static_pointer_cast<Type>(result);
        result = std::make_shared<PointerType>(points_to);
        return success();
    }

    bool Translator::getArrayType(const std::shared_ptr<ArrayTypeNode>& atnode, bool includeOpaquePtr) {

        if (!getType(atnode->array_of, includeOpaquePtr)) {
            return error(atnode->array_of, "Unable to create array element type");
        }
        auto array_of = std::static_pointer_cast<Type>(result);
        if (array_of->isFunction()) {
            return error(atnode->array_of, "Array element cannot be a function");
        }
        if (!getValue(atnode->nelements)) {
            return error(atnode->nelements, "Unable to evaluate array size");
        }
        auto nelem = std::static_pointer_cast<Value>(result);
        if (!nelem->isConstNoRelocation()) {
            return error(atnode->nelements, "Array size expression does not have a determinate constant value");
        }
        if (!nelem->isConstNonNegativeInt() || nelem->getUInt64ValueOrZero() == 0) {
            return error(atnode->nelements, "Element index is not a positive integer constant");
        }
        result = std::make_shared<ArrayType>(array_of, nelem->getUInt64ValueOrZero());
        return success();
    }

    bool Translator::getStructType(const std::shared_ptr<StructTypeNode>& stnode, bool includeOpaquePtr) {

        std::vector<NameType> members;
        bool packed = stnode->isPacked();
        for (std::size_t i = 0; i < stnode->members->set.size(); i++) {
            const NameNode& m = stnode->members->set[i];
            if (m.name && ast->representations.find(m.name->name) != ast->representations.end()) {
                return error(m.name, "\'" + m.name->name + "\' is also defined as a representation");
            }
            if (!getType(m.node, includeOpaquePtr)) {
                std::string memid = (m.name ? m.name->name : "at index " + std::to_string(i+1));
                return error(&m, "Unable to create struct member " + memid);
            }
            auto ty = std::static_pointer_cast<Type>(result);
            if (ty->isFunction()) {
                return error(m.node, "Struct element cannot be a function");
            }
            members.push_back(NameType(m.name, ty));
        }
        result = std::make_shared<StructType>(members, packed);
        return success();
    }

    bool Translator::getUnionType(const std::shared_ptr<UnionTypeNode>& utnode, bool includeOpaquePtr) {

        std::vector<NameType> members;
        for (std::size_t i = 0; i < utnode->members->set.size(); i++) {
            const NameNode& m = utnode->members->set[i];
            if (m.name && ast->representations.find(m.name->name) != ast->representations.end()) {
                return error(m.name, "\'" + m.name->name + "\' is also defined as a representation");
            }
            if (!getType(m.node, includeOpaquePtr)) {
                std::string memid = (m.name ? m.name->name : "at index " + std::to_string(i+1));
                return error(&m, "Unable to create union member " + memid);
            }
            auto ty = std::static_pointer_cast<Type>(result);
            if (ty->isFunction()) {
                return error(m.node, "Union element cannot be a function");
            }
            members.push_back(NameType(m.name, ty));
        }
        result = std::make_shared<UnionType>(members);
        return success();
    }

    bool Translator::getFunctionType(const std::shared_ptr<FunctionTypeNode>& ftnode, bool includeOpaquePtr) {

        std::vector<NameType> args;
        for (std::size_t i = 0; i < ftnode->args->set.size(); i++) {
            const NameNode& a = ftnode->args->set[i];
            if (ast->representations.find(a.name->name) != ast->representations.end()) {
                return error(a.name, "\'" + a.name->name + "\' is also defined as a representation");
            }
            if (!getType(a.node, includeOpaquePtr)) {
                return error(&a, "Unable to create function argument " + a.name->name);
            }
            auto ty = std::static_pointer_cast<Type>(result);
            if (ty->isFunction()) {
                return error(a.node, "Function argument cannot be a function");
            }
            args.push_back(NameType(a.name, ty, a.attr));
        }
        if (!getType(ftnode->ret, includeOpaquePtr)) {
            return error(ftnode->ret, "Unable to create function return type");
        }
        auto ret = std::static_pointer_cast<Type>(result);
        if (ret->isFunction()) {
            return error(ftnode->ret, "Function return type cannot be a function");
        }
        result = std::make_shared<FunctionType>(args, ret);
        return success();
    }

}
