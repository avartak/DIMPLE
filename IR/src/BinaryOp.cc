#include <llvm/IR/Constants.h>
#include <BinaryOp.h>
#include <UnaryOp.h>
#include <MemoryOp.h>
#include <PrimitiveType.h>
#include <PointerType.h>
#include <ArrayType.h>
#include <StructType.h>
#include <UnionType.h>
#include <Statement.h>
#include <Globals.h>

namespace avl {

    std::shared_ptr<Value> BinaryOp::add(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateAdd(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateFAdd(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::subtract(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret; 
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateSub(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateFSub(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::multiply(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateMul(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateFMul(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::divide(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (checkValidDivision(le, re) != UB_NONE) {
            return ret;
        }
        if (le->type->isInt()) {
            if (le->type->isUnsignedInt()) {
                return std::make_shared<Value>(le->type, TheBuilder.CreateUDiv(le->val(), re->val()));
            }
            else {
                return std::make_shared<Value>(le->type, TheBuilder.CreateSDiv(le->val(), re->val()));
            }
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateFDiv(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::remainder(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (checkValidDivision(le, re) != UB_NONE) {
            return ret;
        }
        if (le->type->isInt()) {
            if (le->type->isUnsignedInt()) {
                return std::make_shared<Value>(le->type, TheBuilder.CreateURem(le->val(), re->val()));
            }
            else {
                return std::make_shared<Value>(le->type, TheBuilder.CreateSRem(le->val(), re->val()));
            }
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateFRem(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::equal(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isBool()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpEQ(le->val(), re->val()));
        }
        else if (le->type->isInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpEQ(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpOEQ(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpEQ(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::unequal(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isBool()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpNE(le->val(), re->val()));
        }
        else if (le->type->isInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpNE(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpUNE(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpNE(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::greater(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isUnsignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpUGT(le->val(), re->val()));
        }
        else if (le->type->isSignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpSGT(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpOGT(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpUGT(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::lesser(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isUnsignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpULT(le->val(), re->val()));
        }
        else if (le->type->isSignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpSLT(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpOLT(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpULT(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::greaterEqual(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isUnsignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpUGE(le->val(), re->val()));
        }
        else if (le->type->isSignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpSGE(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpOGE(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpUGE(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::lesserEqual(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        auto ty = std::make_shared<PrimitiveType>(TYPE_BOOL);
        if (le->type->isUnsignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpULE(le->val(), re->val()));
        }
        else if (le->type->isSignedInt()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpSLE(le->val(), re->val()));
        }
        else if (le->type->isReal()) {
            return std::make_shared<Value>(ty, TheBuilder.CreateFCmpOLE(le->val(), re->val()));
        }
        else if (le->type->isPtr()) {
            auto li = recast(le, std::make_shared<PrimitiveType>(TYPE_UINT64));
            auto ri = recast(re, std::make_shared<PrimitiveType>(TYPE_UINT64));
            return std::make_shared<Value>(ty, TheBuilder.CreateICmpULE(li->val(), ri->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::shiftLeft(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateShl(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::shiftRight(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateLShr(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::bitAnd(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateAnd(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::bitXor(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret; 
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateXor(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::bitOr(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret; 
        if (*le->type != *re->type) {
            return ret;
        }
        if (le->type->isInt()) {
            return std::make_shared<Value>(le->type, TheBuilder.CreateOr(le->val(), re->val()));
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::logAnd(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret;
        if (!le->type->isBool() || !re->type->isBool()) {
            return ret;
        }
        return std::make_shared<Value>(le->type, TheBuilder.CreateAnd(le->val(), re->val()));
    }

    std::shared_ptr<Value> BinaryOp::logOr(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {
        std::shared_ptr<Value> ret; 
        if (!le->type->isBool() || !re->type->isBool()) {
            return ret;
        }
        return std::make_shared<Value>(le->type, TheBuilder.CreateOr(le->val(), re->val()));
    }

    std::shared_ptr<Value> BinaryOp::element(const std::shared_ptr<Variable>& var, const std::shared_ptr<Value>& i) {
        std::shared_ptr<Value> ret;

        if (!i->type->isInt()) {
            return ret;
        }

        if (var->type->isStruct() || var->type->isUnion()) {
            if (!i->isConstNonNegativeInt()) {
                return ret;
            }
            auto idx = i->getUInt64ValueOrZero();

            if (var->type->isStruct()) {
                auto st = static_cast<StructType*>(var->type.get());
                if (idx >= st->members.size()) {
                    return ret;
                }
                auto gep = TheBuilder.CreateStructGEP(var->ptr(), idx);
                auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", st->members[idx].type);
                v->llvm_value = TheBuilder.CreateBitCast(gep, llvm::PointerType::get(v->type->llvm_type, 0));
                ret = v;
            }
            else if (var->type->isUnion()) {
                auto ut = static_cast<UnionType*>(var->type.get());
                if (idx >= ut->members.size()) {
                    return ret;
                }
                auto t = std::make_shared<PointerType>(ut->members[idx].type);
                auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", t->points_to);
                v->llvm_value = TheBuilder.CreateBitCast(var->ptr(), t->llvm_type);
                ret = v;
            }
        }

        if (var->type->isArray() || var->type->isPtr()) {

            if (var->type->isArray()) {
                auto at = static_cast<ArrayType*>(var->type.get());
                std::vector<llvm::Value*> vidx;
                vidx.push_back(TheBuilder.getInt32(0));
                if (at->name != "" && llvm::isa<llvm::StructType>(at->llvm_type)) {
                    vidx.push_back(TheBuilder.getInt32(0));
                }
                vidx.push_back(i->val());
                auto gep = TheBuilder.CreateInBoundsGEP(var->type->llvm_type, var->ptr(), vidx);
                auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", at->array_of);
                v->llvm_value = TheBuilder.CreateBitCast(gep, llvm::PointerType::get(v->type->llvm_type, 0));
                ret = v;
            }
            else if (var->type->isPtr()) {
                auto pt = static_cast<PointerType*>(var->type.get());
                auto gep = TheBuilder.CreateInBoundsGEP(pt->points_to->llvm_type, var->val(), i->val());
                auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", pt->points_to);
                v->llvm_value = TheBuilder.CreateBitCast(gep, llvm::PointerType::get(v->type->llvm_type, 0));
                ret = v;
            }
        }

        return ret;
    }

    std::shared_ptr<Value> BinaryOp::member(const std::shared_ptr<Variable>& var, const std::string& name) {
        std::shared_ptr<Value> ret;

        if (!var->type->isStruct() && !var->type->isUnion()) {
            return ret;
        }

        if (var->type->isStruct()) {
            auto st = static_cast<StructType*>(var->type.get());
            bool found = false;
            std::size_t idx = 0;
            for (std::size_t i = 0; i < st->members.size(); i++) {
                if (st->members[i].name->name == name) {
                    idx = i;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return ret;
            }
            auto gep = TheBuilder.CreateStructGEP(var->ptr(), idx);
            auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", st->members[idx].type);
            v->llvm_value = TheBuilder.CreateBitCast(gep, llvm::PointerType::get(v->type->llvm_type, 0));
            ret = v;
        }
        else if (var->type->isUnion()) {
            auto ut = static_cast<UnionType*>(var->type.get());
            bool found = false;
            std::size_t idx = 0; 
            for (std::size_t i = 0; i < ut->members.size(); i++) {
                if (ut->members[i].name->name == name) {
                    idx = i;
                    found = true;
                    break;
                }
            }
            if (!found) {
                return ret;
            }
            auto t = std::make_shared<PointerType>(ut->members[idx].type);
            auto v = std::make_shared<Variable>(STORAGE_UNDEFINED, "", t->points_to);
            v->llvm_value = TheBuilder.CreateBitCast(var->ptr(), t->llvm_type);
            ret = v;
        }

        return ret;
    }

    std::shared_ptr<Value> BinaryOp::recast(const std::shared_ptr<Value>& e, const std::shared_ptr<Type>& ty) {
        
        std::shared_ptr<Value> ret;
        auto t = ty->clone();
        if (t->isPtr()) {
            t->llvm_type = llvm::PointerType::get(static_cast<PointerType*>(t.get())->points_to->llvm_type, 0); 
        }

        if (*e->type == *t) {
            return std::make_shared<Value>(e->type, e->val());
        }

        llvm::Value* v;
        auto lt = t->llvm_type;   

        if (t->isInt()) {
            if (e->type->isPtr()) {
                v = TheBuilder.CreatePtrToInt(e->val(), lt);
            }
            else if (e->type->isBool()) {
                v = TheBuilder.CreateZExt(e->val(), lt);
            }
            else if (e->type->isInt()) {
                if (e->type->isSignedInt()) {
                    v = TheBuilder.CreateSExtOrTrunc(e->val(), lt);
                }
                else {
                    v = TheBuilder.CreateZExtOrTrunc(e->val(), lt);
                }
            }
            else if (e->type->isReal()) {
                if (t->isSignedInt()) {
                    v = TheBuilder.CreateFPToSI(e->val(), lt);
                }
                else {
                    v = TheBuilder.CreateFPToUI(e->val(), lt);
                }
            }
        }

        else if (t->isReal()) {
            if (e->type->isPtr()) {
                v = TheBuilder.CreatePtrToInt(e->val(), TheBuilder.getInt64Ty());
                v = TheBuilder.CreateUIToFP(v, lt);
            }
            else if (e->type->isBool()) {
                v = TheBuilder.CreateUIToFP(v, lt);
            }
            else if (e->type->isInt()) {
                if (e->type->isSignedInt()) {
                    v = TheBuilder.CreateSIToFP(e->val(), lt);
                }
                else {
                    v = TheBuilder.CreateUIToFP(e->val(), lt);
                }
            }
            else if (e->type->isReal()) {
                if (e->type->is == TYPE_REAL64 && t->is == TYPE_REAL32) {
                    v = TheBuilder.CreateFPTrunc(e->val(), lt);
                }
                else if (e->type->is == TYPE_REAL32 && t->is == TYPE_REAL64) {
                    v = TheBuilder.CreateFPExt(e->val(), lt);
                }
            }
        }

        else if (t->isPtr()) {
            if (e->type->isPtr()) {
                v = TheBuilder.CreateBitCast(e->val(), lt);
            }
            else if (e->type->isBool() || e->type->isInt()) {
                v = TheBuilder.CreateIntToPtr(e->val(), lt);
            }
            else if (e->type->isReal()) {
                v = TheBuilder.CreateFPToUI(e->val(), TheBuilder.getInt64Ty());
                v = TheBuilder.CreateIntToPtr(v, lt);
            }
        }

        else if (t->isBool()) {
            if (e->type->isPtr()) {
                v  = TheBuilder.CreateICmpNE(e->val(), llvm::Constant::getNullValue(e->type->llvm_type));
            }
            else if (e->type->isBool()) {
                v = e->val();
            }
            else if (e->type->isInt()) {
                v = TheBuilder.CreateICmpNE(e->val(), TheBuilder.getFalse());
            }
            else if (e->type->isReal()) {
                v = TheBuilder.CreateFCmpUNE(e->val(), llvm::ConstantFP::get(e->type->llvm_type, 0));  // UNE : unordered (NAN) or not equal ; NAN --> true
            }
        }

        if (v != nullptr) {
            ret = std::make_shared<Value>(t, v);
        }
        return ret;
    }

    std::shared_ptr<Value> BinaryOp::recastImplicit(const std::shared_ptr<Value>& e, const std::shared_ptr<Type>& ty) {

        std::shared_ptr<Value> ret;

        if (e->type->isPtr() && ty->isPtr()) {
            auto  pt = static_cast<PointerType*>(ty.get());
            auto ept = static_cast<PointerType*>(e->type.get());
            if (ept->points_to->isUnknown() || pt->points_to->isUnknown()) {
                return BinaryOp::recast(e, ty);
            }
        }

        if (*e->type == *ty) {
            return e;
        }

        return ret;

    }
        
    std::shared_ptr<Value> BinaryOp::assign(const std::shared_ptr<Variable>& var, const std::shared_ptr<Value>& ex) {

        std::shared_ptr<Value> fail;

        if (*ex->type != *var->type) {
            return fail;
        }

        if (ex->type->moveDirectly()) {
            TheBuilder.CreateStore(ex->val(), var->ptr());
            return var;
        }

        if (!ex->isVar()) {
            return fail;
        }
        auto varex = std::static_pointer_cast<Variable>(ex);
        if (!MemoryOp::memcpy(var, varex)) {
            return fail;
        }
        return var;

    }

    uint16_t BinaryOp::checkValidDivision(const std::shared_ptr<Value>& le, const std::shared_ptr<Value>& re) {

        if (re->type->isInt()) {
            if (re->isConst()) {
                if (re->getUInt64ValueOrZero() == 0) {
                    return UB_DIV_ZERO;
                }
            }
            if (re->type->isSignedInt() && le->isConst() && re->isConst()) {
                if (le->getInt64ValueOrZero() == INT64_MIN && re->getInt64ValueOrZero() == -1) {
                    return UB_DIV_OVERFLOW;
                }
            }
        }
        else if (re->type->isReal()) {
            if (re->isConst()) {
                auto rc = llvm::cast<llvm::ConstantFP>(re->val());
                if (rc->getValue().convertToDouble() == 0.0) {
                    return UB_DIV_ZERO;
                }
            }
        }
        return UB_NONE;

    }

}
