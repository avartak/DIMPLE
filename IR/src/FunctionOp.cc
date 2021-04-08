#include <FunctionOp.h>
#include <BinaryOp.h>
#include <FunctionType.h>
#include <PrimitiveType.h>
#include <Statement.h>
#include <CodeBlock.h>
#include <Globals.h>

namespace avl {

    std::shared_ptr<Value> FunctionOp::call(const std::shared_ptr<Function>& func, const std::vector<std::shared_ptr<Value> >& args) {

        std::shared_ptr<Value> fail;

        auto ft = static_cast<FunctionType*>(func->type.get());
        if (args.size() != ft->args.size()) {
            return fail;
        }

        std::shared_ptr<Variable> retv;
        if (!ft->ret->retDirectly() || ft->ret->isCompound()) {
            retv = std::make_shared<Variable>(STORAGE_LOCAL, "", ft->ret);
            retv->llvm_value = TheBuilder.CreateAlloca(retv->type->llvm_type);
            retv->align();
            retv->init();
        }

        return call(func, args, retv);
    }

    std::shared_ptr<Value> FunctionOp::call(const std::shared_ptr<Function>& func, const std::vector<std::shared_ptr<Value> >& args, const std::shared_ptr<Variable>& retv) {

        std::shared_ptr<Value> ex;

        auto ft = static_cast<FunctionType*>(func->type.get());
        std::vector<llvm::Value*> fargs;
        if (!ft->ret->retDirectly() || ft->ret->isCompound()) {
            if (!retv) {
                return ex;
            }
        }
        if (!ft->ret->retDirectly()) {
			fargs.push_back(retv->ptr());
        }

        for (std::size_t i = 0; i < args.size(); i++) {
            if (*ft->args[i].type != *args[i]->type) {
                return ex;
            }
            if (ft->args[i].passByRef()) {
                if (!args[i]->isVar()) {
                    return ex;
                }
                fargs.push_back(static_cast<Variable*>(args[i].get())->ptr());
            }
            else if (ft->args[i].type->passDirectly()) {
                fargs.push_back(args[i]->val());
            }
            else {
                const auto& v = std::make_shared<Variable>(STORAGE_LOCAL, "", args[i]->type);
                v->llvm_value = TheBuilder.CreateAlloca(v->type->llvm_type);
                v->align();
                if (!BinaryOp::assign(v, args[i])) {
                    return ex;
                }
                fargs.push_back(v->ptr());
            }
        }

        auto fptr = llvm::cast<llvm::Function>(func->ptr());
        if (ft->ret->retDirectly()) {
            llvm::Value* lv;
            if (fargs.size() == 0) {
                lv = TheBuilder.CreateCall(llvm::cast<llvm::FunctionType>(ft->llvm_type), fptr);
            }
            else {
                lv = TheBuilder.CreateCall(llvm::cast<llvm::FunctionType>(ft->llvm_type), fptr, fargs);
            }
            if (!ft->ret->isCompound()) {
                ex = std::make_shared<Value>(ft->ret, lv);
            }
            else {
                auto ptr = TheBuilder.CreateBitCast(retv->ptr(), TheBuilder.getInt64Ty());
                TheBuilder.CreateStore(lv, ptr);
            }
        }
        else {
            TheBuilder.CreateCall(llvm::cast<llvm::FunctionType>(ft->llvm_type), fptr, fargs);
        }
        if (!ft->ret->retDirectly() || ft->ret->isCompound()) {
            ex = retv;
        }

        return ex;
    }

    bool FunctionOp::ret(const std::shared_ptr<Function>& func, const std::shared_ptr<Value>& retval) {

        auto ft = static_cast<FunctionType*>(func->type.get());
        if (!retval && !ft->ret->isVoid()) {
            return false;
        }
        if (retval && ft->ret->isVoid()) {
            return false;
        }
        
        if (ft->ret->isVoid()) {
            TheBuilder.CreateRetVoid();
            return true;
        }
        
        if (*retval->type != *ft->ret) {
            return false;
        }
        
        llvm::Function* fn = llvm::cast<llvm::Function>(func->ptr());
        
        if (!ft->ret->retDirectly()) {
            if (!BinaryOp::assign(func->retvar, retval)) {
                return false;
            }
            TheBuilder.CreateRetVoid();
            return true;
        }
        
        llvm::Value* v;
        if (ft->ret->isCompound()) {
            if (retval->is != VALUE_VAR) {
                return false;
            }
            auto retvar = std::static_pointer_cast<Variable>(retval);
            auto ptr = TheBuilder.CreateBitCast(retvar->ptr(), TheBuilder.getInt64Ty());
            v = TheBuilder.CreateAlignedLoad(TheBuilder.getInt64Ty(), ptr, llvm::MaybeAlign(8));
        }
        else {
            v = retval->val();
        }
        if (!func->lst->prev) {
            if (func->retvar) {
                TheBuilder.CreateStore(v, func->retvar->ptr());
                CodeBlock::jump(func->retblock);
                CodeBlock::insert(func->retblock);
                TheBuilder.CreateRet(func->retvar->val());
            }
            else {
                TheBuilder.CreateRet(v);
            }
        }
        else {
            if (!func->retvar) {
                auto ty = ft->ret->isCompound() ? std::make_shared<PrimitiveType>(TYPE_UINT64) : ft->ret;
                func->retvar = std::make_shared<Variable>(STORAGE_LOCAL, "", ty);
                func->retblock = std::make_shared<CodeBlock>();
                auto current_insert_pt = TheBuilder.saveIP();
                TheBuilder.SetInsertPoint(fn->getEntryBlock().getFirstNonPHIOrDbgOrLifetime());
                func->retvar->llvm_value = TheBuilder.CreateAlloca(func->retvar->type->llvm_type);
                func->retvar->align();
                TheBuilder.restoreIP(current_insert_pt);
            }
            TheBuilder.CreateStore(v, func->retvar->ptr());
            CodeBlock::jump(func->retblock);
        }
        return true;
    }

}

