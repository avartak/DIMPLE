#include <Analyzer.h>
#include <NameNode.h>
#include <ExprNode.h>
#include <Statement.h>
#include <MemoryOp.h>
#include <BinaryOp.h>
#include <Variable.h>
#include <Literal.h>
#include <Globals.h>

namespace avl {

    bool Analyzer::getGlobalVar(const std::shared_ptr<Identifier>& ident, uint16_t storage, const std::shared_ptr<Type>& type) {

        const auto& n = ident->name;
        if (n == "main") {
            return error("\'main\' can only ne defined as a function");
        }

        std::shared_ptr<Node> defn;
        auto var = std::make_shared<Variable>(storage, n, type);
        if (ast->declarations.find(n) != ast->declarations.end()) {
            defn = std::make_shared<NullInit>(false);
        }
        else {
            defn = ast->definitions[n];
        }
        if (!initGlobal(var, defn)) {
            return error();
        }
        result = gst->variables[n] = var;
        return success();
    }

    bool Analyzer::initGlobal(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

        if (rval->kind == NODE_STATEMENT) {
            auto stmt = static_cast<Statement*>(rval.get());
            if (stmt->is != STATEMENT_DEFINE) {
                return error(stmt, "Invalid variable definition");
            }
            auto defn = std::static_pointer_cast<DefineStatement>(rval);
            auto def = defn->def;
            if (def->kind == NODE_INITIALIZER) {
                if (var->type->isPrimitive() || var->type->isPtr()) {
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
            var->define();
            if (static_cast<NullInit*>(rval.get())->zero) {
                var->init();
            }
        }
        else if (rval->kind == NODE_INITIALIZER || rval->kind == NODE_EXPRNODE || rval->kind == NODE_IDENTIFIER) {
            var->define();
            if (!initConst(var->type, rval)) {
                return error();
            }
            var->initGlobal(std::static_pointer_cast<Value>(result));
        }
        else {
            return error(rval, "Invalid variable initializer");
        }
        result = var;
        return success();
    }

    bool Analyzer::initLocal(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

        if (rval->kind == NODE_STATEMENT) {
            auto stmt = static_cast<Statement*>(rval.get());
            if (stmt->is != STATEMENT_DEFINE) {
                return error(stmt, "Invalid variable definition");
            }
            auto defn = std::static_pointer_cast<DefineStatement>(rval);
            auto def = defn->def;
            if (def->kind == NODE_INITIALIZER) {
                if (var->type->isPrimitive() || var->type->isPtr()) {
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
            var->define();
            if (static_cast<NullInit*>(rval.get())->zero) {
                var->init();
            }
            result = var;
            return success();
        }
        else if (rval->kind == NODE_INITIALIZER) {
            var->define();
            if (!var->type->isCompound()) {
                return error(rval, "Only compound types can be initialized using an initializer");
            }
            auto in = std::static_pointer_cast<Initializer>(rval);
            if (var->type->isArray()) {
                return initLocalArray(var, in);
            }
            else if (var->type->isStruct()) {
                return initLocalStruct(var, in);
            }
            return initLocalUnion(var, in);
        }
        else if (rval->kind == NODE_EXPRNODE || rval->kind == NODE_IDENTIFIER) {
            var->define();
            return assign(var, rval);
        }
        else {
            return error(rval, "Invalid variable initializer");
        }
    }

    bool Analyzer::initLocalArray(const std::shared_ptr<Variable>& var, const std::shared_ptr<Initializer>& in) {
        std::size_t idx = -1;
        for (const auto& ie : in->elements) { 
            if (!getArrayTypeIndex(std::static_pointer_cast<ArrayType>(var->type), ie, idx)) {
                return error();
            }
            const auto& ele = std::make_shared<IntLiteral>(idx);
            const auto& val = BinaryOp::element(var, ele);
            if (!val) {
                return error(&ie, "Array element access failed"); // This should never happen
            }
            auto iv = std::static_pointer_cast<Variable>(val);
            if (!initLocal(iv, ie.value)) {
                return error();
            }
        }
        result = var;
        return success();
    }

    bool Analyzer::initLocalStruct(const std::shared_ptr<Variable>& var, const std::shared_ptr<Initializer>& in) {
        std::size_t idx = -1;
        for (const auto& ie : in->elements) {
            if (!getStructTypeIndex(std::static_pointer_cast<StructType>(var->type), ie, idx)) {
                return error();
            }
            const auto& ele = std::make_shared<IntLiteral>(idx);
            const auto& val = BinaryOp::element(var, ele);
            if (!val) {
                return error(&ie, "Struct member access failed"); // This should never happen 
            }
            auto iv = std::static_pointer_cast<Variable>(val);
            if (!initLocal(iv, ie.value)) {
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
        std::size_t idx = -1;
        if (!getUnionTypeIndex(std::static_pointer_cast<UnionType>(var->type), in->elements[0], idx)) {
            return error();
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

        if (ty->isCompound()) {
            if (nd->kind != NODE_INITIALIZER) {
                return error(nd, "Initializer needed for compound type");
            }
            auto in = std::static_pointer_cast<Initializer>(nd);
            if (ty->isArray()) {
                return initArrayConst(std::static_pointer_cast<ArrayType>(ty), in);
            }
            else if (ty->isStruct()) {
                return initStructConst(std::static_pointer_cast<StructType>(ty), in);
            }
            return initUnionConst(std::static_pointer_cast<UnionType>(ty), in);
        }

        if (nd->kind == NODE_INITIALIZER) {
            return error(nd, "Only compound types can be initialized using an initializer");
        }
        return initSimpleConst(ty, nd);
    }

    bool Analyzer::initSimpleConst(const std::shared_ptr<Type>& ty, const std::shared_ptr<Node>& nd) {

        if (!getValue(nd)) {
            return error(nd, "Unable to obtain initial value");
        }

        auto t = ty;
        if (ty->isPtr()) {
            t = std::make_shared<PointerType>(static_cast<PointerType*>(ty.get())->points_to);
        }

        auto ex = std::static_pointer_cast<Value>(result);
        ex = BinaryOp::recastImplicit(ex, t);
        if (!ex) {
            return error(nd, "Initial value has inconsistent type");
        }
        if (!ex->isConst()) {
            return error(nd, "Initial value is not a constant");
        }

        result = ex;
        return success();
    }

    bool Analyzer::initArrayConst(const std::shared_ptr<ArrayType>& ty, const std::shared_ptr<Initializer>& in) {

        auto t = std::static_pointer_cast<ArrayType>(ty->clone());

        std::size_t idx = -1;
        std::map<std::size_t, std::shared_ptr<Value> > cmap;
        for (const auto& ie : in->elements) {
            if (!getArrayTypeIndex(ty, ie, idx)) {
                return error();
            }
            if (!initConst(t->array_of, ie.value)) {
                return error(ie.value, "Unable to initialize array element at index " + std::to_string(idx));
            }
            cmap[idx] = std::static_pointer_cast<Value>(result);
        }

        result = ArrayType::initConst(t, cmap);
        return success();
    }

    bool Analyzer::initStructConst(const std::shared_ptr<StructType>& ty, const std::shared_ptr<Initializer>& in) {

        auto t = std::static_pointer_cast<StructType>(ty->clone());

        std::size_t idx = -1;
        std::map<std::size_t, std::shared_ptr<Value> > cmap;
        for (const auto& ie : in->elements) {
            if (!getStructTypeIndex(ty, ie, idx)) {
                return error();
            }
            if (!initConst(ty->members[idx].type, ie.value)) {
                std::string memid = (ty->members[idx].name ? ty->members[idx].name->name : "at index " + std::to_string(idx+1));
                return error(ie.value, "Unable to initialize struct member " + memid);
            }
            cmap[idx] = std::static_pointer_cast<Value>(result);
        }
        result = StructType::initConst(t, cmap);
        return success();
    }

    bool Analyzer::initUnionConst(const std::shared_ptr<UnionType>& ty, const std::shared_ptr<Initializer>& in) {

        auto t = std::static_pointer_cast<UnionType>(ty->clone());

        if (in->elements.size() > 1) {
            return error(in, "Union initializer with more than one element");
        }
        std::size_t idx = -1;

        if (!getUnionTypeIndex(ty, in->elements[0], idx)) {
            return error();
        }

        if (!initConst(ty->members[idx].type, in->elements[0].value)) {
            std::string memid = (ty->members[idx].name ? ty->members[idx].name->name : "at index " + std::to_string(idx+1));
            return error(in->elements[0].value, "Unable to initialize union member " + memid);
        }
        result = UnionType::initConst(t, std::static_pointer_cast<Value>(result));
        return success();
    }

    bool Analyzer::getArrayTypeIndex(const std::shared_ptr<ArrayType>& t, const InitElement& ie, std::size_t& last_idx) {
        auto idx = last_idx + 1;
        if (ie.is != INIT_UNTAGGED) {
            if (ie.is == INIT_LABELED) {
                return error(&ie, "Labeled initializer for array type");
            }
            if (!getValue(ie.tag)) {
                return error(ie.tag, "Unable to get the index of the initalizer element");
            }
            auto index = static_cast<Value*>(result.get());
            if (!index->isConstNoRelocation()) {
                return error(ie.tag, "Array element index is not a determinate constant");
            }
            if (!index->isConstNonNegativeInt()) {
                return error(ie.tag, "Array element index must be a non-negative integer constant");
            }
            idx = index->getUInt64ValueOrZero();
        }
        if (idx >= t->nelements) {
            return error(&ie, "Array initializer element at index " + std::to_string(idx) + " is out of bounds for array of size " + std::to_string(t->nelements));
        }
        last_idx = idx;
        return success();
    }

    bool Analyzer::getStructTypeIndex(const std::shared_ptr<StructType>& t, const InitElement& ie, std::size_t& last_idx) {
        auto idx = last_idx + 1;
        if (ie.is != INIT_UNTAGGED) {
            if (ie.is == INIT_LABELED) {
                if (ie.tag->kind != NODE_IDENTIFIER) {
                    return error(ie.tag, "Unexpected struct initializer label");
                }
                auto ident = static_cast<const Identifier*>(ie.tag.get());
                bool found = false;
                for (std::size_t j = 0; j < t->members.size(); j++) {
                    if (t->members[j].name && t->members[j].name->name == ident->name) {
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
                    return error("Unable to obtain struct initializer index");
                }
                auto iv = static_cast<Value*>(result.get());
                if (!iv->isConstNoRelocation()) {
                    return error(ie.tag, "Struct initalizer index is not a determinate constant");
                }
                if (!iv->isConstNonNegativeInt()) {
                    return error(ie.tag, "Struct initializer index must be a non-negative integer constant");
                }
                idx = iv->getUInt64ValueOrZero();
            }
        }
        if (idx >= t->members.size()) {
            return error(&ie, "Initializer element out of bounds of the struct");
        }
        last_idx = idx;
        return success();
    }

    bool Analyzer::getUnionTypeIndex(const std::shared_ptr<UnionType>& t, const InitElement& ie, std::size_t& last_idx) {
        auto idx = last_idx;
        if (ie.is == INIT_UNTAGGED) {
            return error(ie, "Union initializer cannot be untagged");
        }
        else if (ie.is == INIT_LABELED) {
            if (ie.tag->kind != NODE_IDENTIFIER) {
                return error(ie.tag, "Unexpected union initializer label");
            }
            auto ident = static_cast<const Identifier*>(ie.tag.get());
            bool found = false;
            for (std::size_t j = 0; j < t->members.size(); j++) {
                if (t->members[j].name && t->members[j].name->name == ident->name) {
                    idx = j;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return error(ie.tag, "No member " + ident->name + " in union");
            }
        }
        else {
            if (!getValue(ie.tag)) {
                return error(ie.tag, "Unable to obtain union initializer index as a compile-time constant");
            }
            auto iv = static_cast<Value*>(result.get());
            if (!iv->isConstNoRelocation()) {
                return error(ie.tag, "Union initalizer index is not a determinate constant");
            }
            if (!iv->isConstNonNegativeInt()) {
                return error(ie.tag, "Union initializer index must be a non-negative integer constant");
            }
            idx = iv->getUInt64ValueOrZero();
            if (idx >= t->members.size()) {
                return error(ie, "Initializer element out of bounds of the union");
            }
        }
        last_idx = idx;
        return success();
    }
}
