#include <sstream>
#include <llvm/IR/Constants.h>
#include <Analyzer.h>
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

    bool Analyzer::getType(const std::shared_ptr<Node>& node, bool includeOpaquePtr) {

        if (node->kind == NODE_IDENTIFIER) {
            auto ident = std::static_pointer_cast<Identifier>(node);
            const auto& n = ident->name;

            if (ast->representations.find(n) == ast->representations.end()) {
                return error(ident, "Identifier " + n + " is not a representation");
            }

            if (gst->types.find(n) != gst->types.end()) {
                if (gst->types[n]->isComplete() || includeOpaquePtr) {
                    result = gst->types[n];
                    return success();
                }
                else {
                    return error(ident, "Type " + n + " is incomplete");
                }
            }

            auto nsnode = ast->getNonSynonymRepNode(ident);
            if (nsnode->kind == NODE_IDENTIFIER) {
                return error(nsnode, "\'" + static_cast<Identifier*>(nsnode.get())->name + "\' is not a representation");
            }
            if (nsnode->kind != NODE_TYPENODE) {
                return error();
            }
            auto tnode = std::static_pointer_cast<TypeNode>(nsnode);

            if (tnode->isPrimitive()) {
                gst->types[n] = std::make_shared<PrimitiveType>(tnode->is);
            }
            else if (tnode->isUnknown()) {
                gst->types[n] = std::make_shared<UnknownType>();
            }
            else if (tnode->isStruct()) {
                gst->types[n] = std::make_shared<StructType>(n, static_cast<StructTypeNode*>(tnode.get())->isPacked());
            }
            else if (tnode->isUnion()) {
                gst->types[n] = std::make_shared<UnionType>(n);
            }
            else if (tnode->isPtr()) {
                gst->types[n] = std::make_shared<PointerType>(n);
            }
            else if (tnode->isArray()) {
                gst->types[n] = std::make_shared<ArrayType>(n);
            }
            else if (tnode->isFunction()) {
                gst->types[n+".ptr"] = std::make_shared<PointerType>(n);
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

        if (node->kind != NODE_TYPENODE) {
            return error();
        }
        auto tnode = std::static_pointer_cast<TypeNode>(node);

        if (tnode->isPrimitive()) {
            result = std::make_shared<PrimitiveType>(tnode->is);
            return success();
        }
        else if (tnode->isUnknown()) {
            result = std::make_shared<UnknownType>();
            return success();
        }
        else if (tnode->isVoid()) {
            result = std::make_shared<VoidType>();
            return success();
        }
        else if (tnode->isPtr()) {
            return getPtrType(std::static_pointer_cast<PointerTypeNode>(tnode), includeOpaquePtr);
        }
        else if (tnode->isArray()) {
            return getArrayType(std::static_pointer_cast<ArrayTypeNode>(tnode), includeOpaquePtr);
        }
        else if (tnode->isStruct()) {
            return getStructType(std::static_pointer_cast<StructTypeNode>(tnode), includeOpaquePtr);
        }
        else if (tnode->isUnion()) {
            return getUnionType(std::static_pointer_cast<UnionTypeNode>(tnode), includeOpaquePtr);
        }
        else if (tnode->isFunction()) {
            return getFunctionType(std::static_pointer_cast<FunctionTypeNode>(tnode), includeOpaquePtr);
        }
        else {
           return error(tnode, "Unable to decipher type");
        }
    }

    bool Analyzer::getPtrType(const std::shared_ptr<PointerTypeNode>& tnode, bool includeOpaquePtr) {

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

    bool Analyzer::getArrayType(const std::shared_ptr<ArrayTypeNode>& atnode, bool includeOpaquePtr) {

        if (!getType(atnode->array_of, includeOpaquePtr)) {
            return error(atnode->array_of, "Unable to create array element type");
        }
        auto array_of = std::static_pointer_cast<Type>(result);
        if (!useInArraySizeExpr(atnode->nelements)) {
            return error(atnode->nelements, "Array index does not have a determinate constant value");
        }
        if (!getValue(atnode->nelements)) {
            return error(atnode->nelements, "Unable to evaluate array size");
        }
        auto nv = std::static_pointer_cast<Value>(result);
        if (!nv->type->isInt()) {
            return error(atnode->nelements, "Array index is not an integer");
        }
        if (!nv->isConst()) {
            return error(atnode->nelements, "Array index is not a compile time constant");
        }
        auto ci = llvm::cast<llvm::ConstantInt>(nv->val());
        if (ci->isNegative()) {
            return error(atnode->nelements, "Array index is negative");
        }
        auto nelem = ci->getZExtValue();
        result = std::make_shared<ArrayType>(array_of, nelem);
        return success();
    }

    bool Analyzer::getStructType(const std::shared_ptr<StructTypeNode>& stnode, bool includeOpaquePtr) {

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
            members.push_back(NameType(m.name, std::static_pointer_cast<Type>(result)));
        }
        result = std::make_shared<StructType>(members, packed);
        return success();
    }

    bool Analyzer::getUnionType(const std::shared_ptr<UnionTypeNode>& utnode, bool includeOpaquePtr) {

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
            members.push_back(NameType(m.name, std::static_pointer_cast<Type>(result)));
        }
        result = std::make_shared<UnionType>(members);
        return success();
    }

    bool Analyzer::getFunctionType(const std::shared_ptr<FunctionTypeNode>& ftnode, bool includeOpaquePtr) {

        std::vector<NameType> args;
        for (std::size_t i = 0; i < ftnode->args->set.size(); i++) {
            const NameNode& a = ftnode->args->set[i];
            if (ast->representations.find(a.name->name) != ast->representations.end()) {
                return error(a.name, "\'" + a.name->name + "\' is also defined as a representation");
            }
            if (!getType(a.node, includeOpaquePtr)) {
                return error(&a, "Unable to create function argument " + a.name->name);
            }
            args.push_back(NameType(a.name, std::static_pointer_cast<Type>(result)));
        }
        if (!getType(ftnode->ret, includeOpaquePtr)) {
            return error(ftnode->ret, "Unable to create function return type");
        }
        auto ret = std::static_pointer_cast<Type>(result);
        result = std::make_shared<FunctionType>(args, ret);
        return success();
    }

    /*
    We do not want to be using indeterminate compile time constants 
    in the expression involving the size of the array
    The only source of such constants is global instances (pointers to them)
    Any identifier that is not a representation would be bad
    A string literal would also be bad
    Call or assign expressions would obviously be bad
    */ 
    bool Analyzer::useInArraySizeExpr(const std::shared_ptr<Node>& node) {
        if (node->kind == NODE_IDENTIFIER) {
            auto ident = static_cast<Identifier*>(node.get());
            const auto& n = ident->name;
            if (ast->representations.find(n) != ast->representations.end()) {
                return useInArraySizeExpr(ast->representations[n]->node);
            }
            return false;
        }

        if (node->kind != NODE_EXPRNODE) {
            return true;
        }
        auto exnode = static_cast<ExprNode*>(node.get());
        if (exnode->is == EXPR_UNARY) {
            auto unary = static_cast<UnaryExprNode*>(node.get());
            if (unary->op == UNARYOP_SIZE) {
                return true;
            }
            else {
                return useInArraySizeExpr(unary->exp);
            }
        }
        else if (exnode->is == EXPR_BINARY) {
            auto binary = static_cast<BinaryExprNode*>(node.get());
            return useInArraySizeExpr(binary->lhs) && useInArraySizeExpr(binary->rhs);
        }
        else if (exnode->isLiteralNode()) {
            if (exnode->is == EXPR_STRING) {
                return false;
            }
            return true;
        }
        return false;
    }

}
