#include <llvm/IR/Constants.h>
#include <UnaryOp.h>
#include <BinaryOp.h>
#include <Literal.h>
#include <PointerType.h>
#include <PrimitiveType.h>
#include <UnknownType.h>
#include <Variable.h>
#include <Function.h>
#include <Statement.h>
#include <Globals.h>

namespace dmp {

    std::shared_ptr<Value> UnaryOp::plus(const std::shared_ptr<Value>& e) {
        std::shared_ptr<Value> ue;
        if (!e->type->isInt() && !e->type->isReal()){
             return ue;
        }
        return e;
    }

    std::shared_ptr<Value> UnaryOp::negate(const std::shared_ptr<Value>& e) {
        std::shared_ptr<Value> ue;
        if (!e->type->isInt() && !e->type->isReal()){
             return ue;
        }
        else if (e->type->isInt()) {
            auto zero = llvm::ConstantInt::get(e->type->llvm_type, 0);
            return std::make_shared<Value>(e->type, TheBuilder->CreateSub(zero, e->val()));
        }
        else {
            auto zero = llvm::ConstantFP::get(e->type->llvm_type, 0);
            return std::make_shared<Value>(e->type, TheBuilder->CreateFSub(zero, e->val()));
        }
    }

    std::shared_ptr<Value> UnaryOp::logicalNot(const std::shared_ptr<Value>& e) {
        std::shared_ptr<Value> ue;
        if (!e->type->isBool()) {
            return ue;
        }
        auto zero = llvm::ConstantInt::get(e->type->llvm_type, 0);
        return std::make_shared<Value>(e->type, TheBuilder->CreateICmpEQ(zero, e->val()));
    }

    std::shared_ptr<Value> UnaryOp::complement(const std::shared_ptr<Value>& e) {
        std::shared_ptr<Value> ue;
        if (!e->type->isInt()) {
            return ue;
        }
        auto mone = llvm::ConstantInt::get(e->type->llvm_type, -1);
        return std::make_shared<Value>(e->type, TheBuilder->CreateXor(mone, e->val()));
    }

    std::shared_ptr<Value> UnaryOp::size(const std::shared_ptr<Type>& t) {
        
        std::shared_ptr<Value> ue;
        IntLiteral i(t->size());
        if (i.literal > 0) {
            ue = std::make_shared<Value>(std::make_shared<PrimitiveType>(TYPE_UINT64), i.val());
        }
        return ue;
    }

    std::shared_ptr<Value> UnaryOp::address(const std::shared_ptr<Value>& e) {

        std::shared_ptr<Value> ue;

        if (e->isInstance()) {
            auto inst = static_cast<const Instance*>(e.get());
            auto type = std::make_shared<PointerType>(inst->type);
            ue = std::make_shared<Value>(type, inst->ptr());
        }
        else if (e->type->isInt() && e->isConst()) {
            auto type = std::make_shared<PointerType>(std::make_shared<UnknownType>());
            auto recast = BinaryOp::recast(e, type);
            ue = std::make_shared<Value>(type, recast->val());
        }

        return ue;
    }

    std::shared_ptr<Value> UnaryOp::dereference(const std::shared_ptr<Value>& e) {
        
        std::shared_ptr<Value> ue;
        
        if (!e->type->isPtr()) {
            return ue;
        }
        auto pt = static_cast<const PointerType*>(e->type.get());
        if (pt->points_to->isFunction()) {
            auto func = std::make_shared<Function>(STORAGE_REFERENCE, "", pt->points_to);
            func->llvm_value = e->val();
            ue = func;
        }
        else {
            if (pt->points_to->size() == 0) {
                return ue;   
            }
            auto var = std::make_shared<Variable>(STORAGE_REFERENCE, "", pt->points_to);
            var->llvm_value = e->val(); // Should we align here as well ?
            ue = var;
        }
 
        return ue;
    }

    std::shared_ptr<Value> UnaryOp::isTrue(const std::shared_ptr<Value>& e) {
        std::shared_ptr<Value> ue;
        if (!e->type->isBool()) {
            return ue;
        }
        auto zero = llvm::ConstantInt::get(e->type->llvm_type, 0);
        return std::make_shared<Value>(e->type, TheBuilder->CreateICmpNE(zero, e->val()));
    }

    std::shared_ptr<Value> UnaryOp::isFalse(const std::shared_ptr<Value>& e) {
        return logicalNot(e);
    }

}
