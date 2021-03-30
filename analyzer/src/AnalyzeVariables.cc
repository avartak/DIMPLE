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
            if (stat->is != STATEMENT_DEFINE) {
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
            if (stat->is != STATEMENT_DEFINE) {
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

        if (ty->isPrimitive()) {
            return initPrimitiveConst(std::static_pointer_cast<PrimitiveType>(ty), nd);
        }
        else if (ty->isPtr()) {
            return initPtrConst(std::static_pointer_cast<PointerType>(ty), nd);
        }
        else if (ty->isArray()) {
            return initArrayConst(std::static_pointer_cast<ArrayType>(ty), nd);
        }
        else if (ty->isStruct()) {
            return initStructConst(std::static_pointer_cast<StructType>(ty), nd);
        }
        else if (ty->isUnion()) {
            return initUnionConst(std::static_pointer_cast<UnionType>(ty), nd);
        }

        return error(nd, "Failed to construct initializer; type is unrecognizable"); // This should never happen

    }

    bool Analyzer::initPrimitiveConst(const std::shared_ptr<PrimitiveType>& ty, const std::shared_ptr<Node>& nd) {

        if (nd->kind == NODE_INITIALIZER) {
            return error(nd, "Only compound types can be initialized using an initializer");
        }
        else {
            if (!getValue(nd)) {
                return error(nd, "Unable to construct initializer");
            }
        }

        auto ex = std::static_pointer_cast<Value>(result);
        if (!ex->isConst()) {
            return error(nd, "Global initializer is not a constant");
        }
        else if (*ty != *ex->type) {
            return error(nd, "Initializer has inconsistent type");
        }

        result = std::make_shared<Value>(ty, ex->llvm_value);
        return success();
    }

    bool Analyzer::initPtrConst(const std::shared_ptr<PointerType>& ty, const std::shared_ptr<Node>& nd) {

        auto t = std::make_shared<PointerType>(ty->points_to);

        if (nd->kind == NODE_INITIALIZER) {
            return error(nd, "Only compound types can be initialized using an initializer");
        }
        else {
            if (!getValue(nd)) {
                return error(nd, "Unable to construct initializer");
            }
        }

        auto ex = std::static_pointer_cast<Value>(result);
        if (!ex->isConst()) {
            return error(nd, "Global initializer not a constant");
        }

        if (!ex->type->isPtr()) {
            return error(nd, "Initializer inconsistent with a pointer type");
        }
        auto ept = static_cast<PointerType*>(ex->type.get());
        if (t->points_to->isUnknown() || ept->points_to->isUnknown()) {
            ex = BinaryOp::recast(ex, t);
        }
        else if (*t != *ex->type) {
            return error(nd, "Initializer has inconsistent type");
        }

        result = std::make_shared<Value>(t, ex->llvm_value);
        return success();
    }

    bool Analyzer::initArrayConst(const std::shared_ptr<ArrayType>& ty, const std::shared_ptr<Node>& nd) {

        auto t = ty->clone();

        if (nd->kind != NODE_INITIALIZER) {
            return error(nd, "Unable to construct array initializer");
        }
        auto in = static_cast<const Initializer*>(nd.get());
        auto at = static_cast<ArrayType*>(t.get());

        std::size_t last_idx = -1;
        std::map<std::size_t, llvm::Constant*> cmap;
        std::map<std::size_t, llvm::Type*> tmap;
        std::vector<std::pair<std::size_t, llvm::Constant*> > csv;
        std::vector<std::pair<std::size_t, llvm::Type*> > tsv;
        for (const auto& ie : in->elements) {
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
            if (idx >= at->nelements) {
                return error(&ie, "Array initializer element at index " + std::to_string(idx) + " is out of bounds for array of size " + std::to_string(at->nelements));
            }
            if (!initConst(at->array_of, ie.value)) {
                return error(ie.value, "Unable to initialize array element at index " + std::to_string(idx));
            }
            auto cs = std::static_pointer_cast<Value>(result);
            cmap[idx] = llvm::cast<llvm::Constant>(cs->val());
            tmap[idx] = cs->type->llvm_type;
        }
        for (auto icmap : cmap) {
            csv.push_back(icmap);
        }
        for (auto itmap : tmap) {
            tsv.push_back(itmap);
        }

        struct {
            bool operator()(std::pair<std::size_t, llvm::Constant*> a, std::pair<std::size_t, llvm::Constant*> b) const {
                return a.first < b.first;
            }
        } csorter;
        struct {
            bool operator()(std::pair<std::size_t, llvm::Type*> a, std::pair<std::size_t, llvm::Type*> b) const {
                return a.first < b.first;
            }
        } tsorter;

        std::sort(csv.begin(), csv.end(), csorter);
        std::sort(tsv.begin(), tsv.end(), tsorter);

        std::vector<llvm::Type*> tv;
        std::vector<llvm::Constant*> cv;
        #define FILLNULL(n)  do { tv.push_back(llvm::ArrayType::get(at->array_of->llvm_type, n)); \
                                  cv.push_back(llvm::Constant::getNullValue(tv.back())); \
                                } while (false)
        for (std::size_t i = 0; i < csv.size(); i++) {
            if (i == 0 && csv[i].first > 0) {
                FILLNULL(csv[i].first);
            }
            cv.push_back(csv[i].second);
            tv.push_back(tsv[i].second);
            if (i < csv.size()-1) {
                if (csv[i+1].first-1-csv[i].first > 0) {
                    FILLNULL(csv[i+1].first-1-csv[i].first);
                }
            }
            else if (csv[i].first < at->nelements-1) {
                FILLNULL(at->nelements-1-csv[i].first);
            }
        }

        t->llvm_type = llvm::StructType::get(TheContext, tv, true);
        result = std::make_shared<Value>(t, llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(t->llvm_type), cv));
        return success();

    }

    bool Analyzer::initStructConst(const std::shared_ptr<StructType>& ty, const std::shared_ptr<Node>& nd) {

        auto t = ty->clone();

        if (nd->kind != NODE_INITIALIZER) {
            return error(nd, "Unable to construct initializer");
        }
        auto in = static_cast<const Initializer*>(nd.get());

        std::size_t last_idx = -1;
        std::vector<llvm::Constant*> csv;
        std::vector<llvm::Type*> tsv;
        for (auto im : ty->members) {
            tsv.push_back(im.type->llvm_type);
            csv.push_back(llvm::Constant::getNullValue(im.type->llvm_type));
        }
        for (const auto& ie : in->elements) {
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
                        return error(ie.tag, "Unable to obtain struct initializer index as a compile-time constant");
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
            if (!initConst(ty->members[idx].type, ie.value)) {
                if (ty->members[idx].name->name == "") {
                   return error(ie.value, "Unable to initialize struct member at index " + std::to_string(idx));
                }
                else {
                   return error(ie.value, "Unable to initialize struct member " + ty->members[idx].name->name);
                }
            }
            auto cs = std::static_pointer_cast<Value>(result);
            csv[idx] = llvm::cast<llvm::Constant>(cs->llvm_value);
            tsv[idx] = cs->type->llvm_type;
        }
        t->llvm_type = llvm::StructType::get(TheContext, tsv, ty->isPacked());

        result = std::make_shared<Value>(t, llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(t->llvm_type), csv));
        return success();
    }

    bool Analyzer::initUnionConst(const std::shared_ptr<UnionType>& ty, const std::shared_ptr<Node>& nd) {

        auto t = ty->clone();

        if (nd->kind != NODE_INITIALIZER) {
            return error(nd, "Unable to construct initializer");
        }
        auto in = static_cast<const Initializer*>(nd.get());

        if (in->elements.size() > 1) {
            return error(nd, "Union initializer with more than one element");
        }
        std::size_t idx = 0;
        if (in->elements[0].is == INIT_UNTAGGED) {
            return error(nd, "Union initializer cannot be untagged");
        }
        else if (in->elements[0].is == INIT_LABELED) {
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
        }

        std::vector<llvm::Constant*> csv;
        std::vector<llvm::Type*> tsv;
        if (idx >= ty->members.size()) {
            return error(in, "Initializer element out of bounds of the struct");
        }
        if (!initConst(ty->members[idx].type, in->elements[0].value)) {
            if (ty->members[idx].name->name == "") {
               return error(in->elements[0].value, "Unable to initialize union member at index " + std::to_string(idx));
            }
            else {
               return error(in->elements[0].value, "Unable to initialize union member " + ty->members[idx].name->name);
            }
        }
        auto cs = std::static_pointer_cast<Value>(result);
        csv.push_back(llvm::cast<llvm::Constant>(cs->llvm_value));
        tsv.push_back(cs->type->llvm_type);
        std::size_t size1 = ty->members[idx].type->size();
        std::size_t size2 = ty->size() - size1;
        if (size2 > 0) {
            auto at = llvm::ArrayType::get(TheBuilder.getInt8Ty(), size2);
            csv.push_back(llvm::Constant::getNullValue(at));
            tsv.push_back(at);
        }

        t->llvm_type = llvm::StructType::get(TheContext, tsv, true);
        result = std::make_shared<Value>(t, llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(t->llvm_type), csv));
        return success();
    }
}