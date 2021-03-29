#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <Analyzer.h>
#include <NameNode.h>
#include <ExprNode.h>
#include <Statement.h>
#include <MemoryOp.h>
#include <BinaryOp.h>
#include <Variable.h>
#include <Literal.h>
#include <PrimitiveType.h>
#include <PointerType.h>
#include <ArrayType.h>
#include <StructType.h>
#include <UnionType.h>
#include <Globals.h>

namespace avl {

    bool Analyzer::getGlobalVar(const std::shared_ptr<Identifier>& ident) {

        const auto& n = ident->name;

        if (variables.find(n) != variables.end()) {
            if (!variables.find(n)->second) {
                return error("Unable to completely define variable " + n);
            }
 
            result = variables[n];
            return success();
        }

        if (ast->declarations.find(n) == ast->declarations.end() &&
            ast->definitions.find(n) == ast->definitions.end()) 
        {
            return error();
        }

        std::shared_ptr<Variable> var;
        std::shared_ptr<Node> tnode;
        std::shared_ptr<Node> defn;
        uint16_t storage = STORAGE_EXTERNAL;

        if (ast->declarations.find(n) != ast->declarations.end()) {
            tnode = ast->declarations[n]->node;
        }
        else {
            tnode = ast->definitions[n]->type;
            storage = ast->definitions[n]->storage; 
        }

        if (!getType(tnode, false)) {
            return error(ident, "Unable to determine type of " + n);
        }
        auto type = std::static_pointer_cast<Type>(result);
        if (type->isFunction()) {
            return error();
        }
        if (!type->isComplete()) {
            return error(ident, "Type of " + n + " is not completely defined");
        }

        var = std::make_shared<Variable>(storage, n, type);
        if (ast->declarations.find(n) == ast->declarations.end()) {
            defn = ast->definitions[n];
        }
        if (!initGlobal(var, defn)) {
            return error();
        }
        var->align();
        variables[n] = var;
        result = var;
        return success();
    }

    bool Analyzer::initGlobal(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

        if (!rval) {
            auto linkage = (var->storage == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
            var->llvm_pointer = new llvm::GlobalVariable(*TheModule, var->type->llvm_type, false, linkage, nullptr, var->name);
        }

        else if (rval->kind == NODE_STATEMENT) {
            auto stat = static_cast<Statement*>(rval.get());
            if (stat->is != STATEMENT_DEFINITION) {
                return error();
            }
            auto defn = std::static_pointer_cast<DefineStatement>(rval);
            auto def = defn->def;
            if (var->type->isPrimitive() || var->type->isPtr()) {
                if (def->kind == NODE_INITIALIZER) {
                    auto init = std::static_pointer_cast<Initializer>(def);
                    if (init->elements.size() != 1 || init->elements[0].is != INIT_UNTAGGED) {
                        return error(def, "Invalid variable initializer");
                    }
                    def = init->elements[0].value;
                }
            }
            return initGlobal(var, def);
        }

        else if (rval->kind == NODE_NULLINIT) {
            auto nullinit = llvm::Constant::getNullValue(var->type->llvm_type);
            auto linkage = (var->storage == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
            var->llvm_pointer = new llvm::GlobalVariable(*TheModule, var->type->llvm_type, false, linkage, nullinit, var->name);
        }

        else {
            if (!initConst(var->type, rval)) {
                return error();
            }
            auto in = static_cast<Value*>(result.get());
            auto linkage = (var->storage == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
            var->llvm_pointer = new llvm::GlobalVariable(*TheModule, in->type->llvm_type, false, linkage, llvm::cast<llvm::Constant>(in->llvm_value), var->name);
        }

        result = var;
        return success();
    }

    bool Analyzer::initLocal(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

        if (rval->kind == NODE_STATEMENT) {
            auto stat = static_cast<Statement*>(rval.get());
            if (stat->is != STATEMENT_DEFINITION) {
                return error();
            }
            auto defn = std::static_pointer_cast<DefineStatement>(rval);
            auto def = defn->def;
            if (var->type->isPrimitive() || var->type->isPtr()) {
                if (def->kind == NODE_INITIALIZER) {
                    auto init = std::static_pointer_cast<Initializer>(def);
                    if (init->elements.size() != 1 || init->elements[0].is != INIT_UNTAGGED) {
                        return error(def, "Invalid variable initializer");
                    }
                    def = init->elements[0].value;
                }
            }
            return initLocal(var, def);
        }

        else if (rval->kind == NODE_NULLINIT) {
            auto nullinit = static_cast<NullInit*>(rval.get());
            if (nullinit->zero) {
                if (!var->type->moveDirectly()) {
                    MemoryOp::memset(var, 0);
                }
                else {
                    TheBuilder.CreateStore(llvm::Constant::getNullValue(var->type->llvm_type), var->ptr());
                }
            }
            result = var;
            return success();
        }

        else if (rval->kind == NODE_INITIALIZER) {
            auto in = std::static_pointer_cast<Initializer>(rval);
            if (var->type->isArray()) {
                return initLocalArray(var, in);
            }
            else if (var->type->isStruct()) {
                return initLocalStruct(var, in);
            }
            else if (var->type->isUnion()) {
                return initLocalUnion(var, in);
            }
            else {
                return error(rval, "Only compound types can be initialized using an initializer");
            }
        }

        else {
            return assign(var, rval);
        }
    }

    bool Analyzer::initLocalArray(const std::shared_ptr<Variable>& var, const std::shared_ptr<Initializer>& in) {
        auto ty = static_cast<ArrayType*>(var->type.get());
	    std::size_t last_idx = -1;
        for (std::size_t i = 0; i < in->elements.size(); i++) {
            const auto& ie = in->elements[i];
            auto idx = last_idx + 1;
            if (ie.is != INIT_UNTAGGED) {
                if (ie.is == INIT_LABELED) {
                    return error(&ie, "Labeled initializer for array type");
                }
                if (!getValue(ie.tag)) {
                    return error(ie.tag, "Unable to get the index of the initalizer element");
                }
                auto index = static_cast<Value*>(result.get());
                if (!index->type->isInt()) {
                    return error(ie.tag, "Index of the initalizer element must be an integer");
                }
                auto ci = llvm::cast<llvm::ConstantInt>(index->llvm_value);
                if (ci->isNegative()) {
                    return error(ie.tag, "Index of the initalizer element is negative");
                }
                idx = ci->getZExtValue();
            }
            last_idx = idx;
            if (idx >= ty->nelements) {
                return error(&ie, "Array initializer element at index " + std::to_string(idx) + " is out of bounds for array of size " + std::to_string(ty->nelements));
            }

            const auto& ele = std::make_shared<IntLiteral>(idx);
            const auto& val = BinaryOp::element(var, ele);
            if (!val) {
                return error(&ie, "Array element access failed"); // This should never happen
            }
            auto iv = std::static_pointer_cast<Variable>(val);
            if (!initLocal(iv, in->elements[i].value)) {
                return error();
            }
        }
        result = var;
        return success();
    }

    bool Analyzer::initLocalStruct(const std::shared_ptr<Variable>& var, const std::shared_ptr<Initializer>& in) {
        auto ty = static_cast<StructType*>(var->type.get());
        std::size_t last_idx = -1;
        for (std::size_t i = 0; i < in->elements.size(); i++) {
            const auto& ie = in->elements[i];
            auto idx = last_idx + 1;
            if (ie.is != INIT_UNTAGGED) {
                if (ie.is == INIT_LABELED) {
                    if (ie.tag->kind != NODE_IDENTIFIER) {
                        return error(ie.tag, "Unexpected struct initializer label");
                    }
                    auto ident = static_cast<const Identifier*>(ie.tag.get());
                    bool found = false;
                    for (std::size_t j = 0; j < ty->members.size(); j++) {
                        if (ty->members[j].name->name == ident->name) {
                            idx = j;
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        return error(ie.tag, "No member " + ident->name + " in struct");
                    }
                }
                else {
                    if (!getValue(ie.tag)) {
                        return error("Unable to obtain struct initializer index as a compile-time constant");
                    }
                    auto iv = static_cast<Value*>(result.get());
                    if (!iv->type->isInt()) {
                        return error(ie.tag, "Struct initializer index is not an integer");
                    }
                    if (!iv->isConst()) {
                        return error(ie.tag, "Struct initializer index is not a compile-time constant");
                    }
                    auto ci = llvm::cast<llvm::ConstantInt>(iv->val());
                    if (ci->isNegative()) {
                        return error(ie.tag, "Struct initializer index is negative");
                    }
                    idx = ci->getZExtValue();
                }
            }
            last_idx = idx;
            if (idx >= ty->members.size()) {
                return error(&ie, "Initializer element out of bounds of the struct");
            }

            const auto& ele = std::make_shared<IntLiteral>(idx);
            const auto& val = BinaryOp::element(var, ele);
            if (!val) {
                return error(&ie, "Struct member access failed"); // This should never happen 
            }
            auto iv = std::static_pointer_cast<Variable>(val);
            if (!initLocal(iv, in->elements[i].value)) {
                return error();
            }
        }
        result = var;
        return success();
    }

    bool Analyzer::initLocalUnion(const std::shared_ptr<Variable>& var, const std::shared_ptr<Initializer>& in) {
        auto ty = static_cast<UnionType*>(var->type.get());
        if (in->elements.size() > 1) {
            return error(in, "Union initializer with more than one element");
        }
        std::size_t idx = 0;
        if (in->elements[0].is == INIT_UNTAGGED) {
            return error(in, "Union initializer cannot be untagged");
        }
        if (in->elements[0].is == INIT_LABELED) {
            if (in->elements[0].tag->kind != NODE_IDENTIFIER) {
                return error(in->elements[0].tag, "Unexpected union initializer label");
            }
            auto ident = static_cast<const Identifier*>(in->elements[0].tag.get());
            bool found = false;
            for (std::size_t j = 0; j < ty->members.size(); j++) {
                if (ty->members[j].name->name == ident->name) {
                    idx = j;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return error(in->elements[0].tag, "No member " + ident->name + " in union");
            }
        }
        else {
            if (!getValue(in->elements[0].tag)) {
                return error(in->elements[0].tag, "Unable to obtain union initializer index as a compile-time constant");
            }
            auto iv = static_cast<Value*>(result.get());
            if (!iv->type->isInt()) {
                return error(in->elements[0].tag, "Union initializer index is not an integer");
            }
            if (!iv->isConst()) {
                return error(in->elements[0].tag, "Union initializer index is not a compile-time constant");
            }
            auto ci = llvm::cast<llvm::ConstantInt>(iv->val());
            if (ci->isNegative()) {
                return error(in->elements[0].tag, "Union initializer index is negative");
            }
            idx = ci->getZExtValue();
            if (idx >= ty->members.size()) {
                return error(in, "Union initializer index is out of bounds of the union members set");
            }
        }

        const auto& ele = std::make_shared<IntLiteral>(idx);
        const auto& ie = BinaryOp::element(var, ele);
        if (!ie) {
            return error(in, "Union member access failed"); // This should never happen
        }
        auto iv = std::static_pointer_cast<Variable>(ie);
        if (!initLocal(iv, in->elements[0].value)) {
            return error();
        }
        result = var;
        return success();
    }

    bool Analyzer::initConst(const std::shared_ptr<Type>& ty, const std::shared_ptr<Node>& nd) {

        return error();

    }
}
