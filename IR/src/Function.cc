#include <llvm/IR/CFG.h>
#include <Function.h>
#include <FunctionType.h>
#include <CodeBlock.h>
#include <Statement.h>
#include <Globals.h>

namespace avl {

    Function::Function(int s, const std::string& n, const std::shared_ptr<Type>& t):
        Instance(VALUE_FUNC, s, n, t),
        lst(std::make_shared<LST>()),
        freeze_block(nullptr)
    {
        auto ft = static_cast<FunctionType*>(type.get());
        auto linkage = (s == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
        auto fptr = llvm::Function::Create(llvm::cast<llvm::FunctionType>(type->llvm_type), linkage, n, *TheModule);
        if (!ft->ret->retDirectly()) {
            fptr->getArg(0)->addAttr(llvm::Attribute::StructRet);
            fptr->getArg(0)->addAttr(llvm::Attribute::NoAlias);
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            auto idx = ft->ret->retDirectly() ? i : i+1;
            if (ft->args[i].passByRef()) {
                fptr->getArg(idx)->addAttr(llvm::Attribute::getWithDereferenceableBytes(TheContext, ft->args[i].type->size()));
            }
            else if (!ft->args[i].type->passDirectly()) {
                fptr->getArg(idx)->addAttr(llvm::Attribute::ByVal);
                fptr->getArg(idx)->addAttr(llvm::Attribute::getWithAlignment(TheContext, llvm::Align(8)));
            }
        }
        llvm_value = fptr;
    }

    llvm::Value* Function::val() const {
        return nullptr;
    }

    llvm::Value* Function::ptr() const {
        return llvm_value;
    }

	void Function::init() {
        auto fn = llvm::cast<llvm::Function>(ptr());
        auto ft = static_cast<FunctionType*>(type.get());

        const auto& block = std::make_shared<CodeBlock>(*this);

        if (!ft->ret->retDirectly()) {
            retvar = std::make_shared<Variable>(STORAGE_LOCAL, "", ft->ret);
            retvar->llvm_value = fn->getArg(0);
            retblock = std::make_shared<CodeBlock>();
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            auto idx = ft->ret->retDirectly() ? i : i+1;
            std::shared_ptr<Variable> var;
            if (!ft->args[i].passByRef() && ft->args[i].type->passDirectly()) {
                var = std::make_shared<Variable>(STORAGE_LOCAL, "", ft->args[i].type);
                var->define();
                if (var->type->isCompound()) {
                    auto u64 = TheBuilder.CreateBitCast(var->ptr(), TheBuilder.getInt64Ty());
                    TheBuilder.CreateStore(fn->getArg(idx), u64);
                }
                else {
                    TheBuilder.CreateStore(fn->getArg(idx), var->ptr());
                }
            }
            else {
                var = std::make_shared<Variable>(STORAGE_UNDEFINED, "", ft->args[i].type);
                var->llvm_value = fn->getArg(idx);
            }
            args.push_back(var);
            lst->variables[ft->args[i].name->name] = var;
        }
	}

    void Function::resetLocals() {
        if (lst->prev) {
            lst = lst->prev;
        }
    }

    void Function::freeze() {
        freeze_block = TheBuilder.GetInsertBlock();
    }

    bool Function::resume() {
        if (freeze_block != nullptr) {
            TheBuilder.SetInsertPoint(freeze_block);
            return true;
        }
        return false;
    }

    bool Function::checkTerminations() {

        auto fn = llvm::cast<llvm::Function>(ptr());
		bool status = true;
        freeze();

        if (retvar && retblock && retblock->block->getParent() == nullptr) {
            CodeBlock::insert(retblock);
		    TheBuilder.CreateRet(retvar->val());
        }

        for (auto iter = fn->getBasicBlockList().begin(); iter != fn->getBasicBlockList().end(); iter++) {
            if (iter->getTerminator() != nullptr) {
                continue;
            }
			TheBuilder.SetInsertPoint(&*iter);
            if (fn->getReturnType()->isVoidTy()) {
                TheBuilder.CreateRetVoid();
            }
            else if (retvar && freeze_block == &*iter) {
                TheBuilder.CreateRet(retvar->val());
            }
			else {
                status = false;
            }
        }

        resume();
        return status;
    }

    /*
    Taken from : https://github.com/llvm/llvm-project/blob/main/llvm/examples/IRTransforms/SimplifyCFG.cpp
    This code does not preserve the DominatorTree
    */

    void Function::simplify() {

        auto fn = llvm::cast<llvm::Function>(ptr());

        // Remove dead block with no predecessors
        bool found = false;
        for (auto& BB : make_early_inc_range(*fn)) {
            if (&BB == &fn->getEntryBlock() || BB.hasNPredecessorsOrMore(1)) {
                continue;
            }
            for (llvm::BasicBlock* succ : successors(&BB)) {
                succ->removePredecessor(&BB);
            }
            while (!BB.empty()) {
                auto& inst = BB.back();
                inst.replaceAllUsesWith(llvm::UndefValue::get(inst.getType()));
                inst.eraseFromParent();
            }
            BB.eraseFromParent();
            found = true;
        }
        if (found) {
            simplify();
        }

        // Remove branches with constant conditionals
        found = false;
        for (auto& BB : *fn) {
            auto BI = llvm::dyn_cast<llvm::BranchInst>(BB.getTerminator());
            if (!BI || !BI->isConditional()) {
                continue;
            }
            
            auto CI = llvm::dyn_cast<llvm::ConstantInt>(BI->getCondition());
            if (!CI) {
                continue;
            }
            
            auto RemovedSucc = BI->getSuccessor(CI->isOne());
            RemovedSucc->removePredecessor(&BB);
            
            llvm::BranchInst::Create(BI->getSuccessor(CI->isZero()), BI);
            BI->eraseFromParent();
            found = true;
        }
        if (found) {
            simplify();
        }
       
        // Merge block into its single predecessor, if the predecessor has a single successor
        found = false;
        for (auto& BB : make_early_inc_range(*fn)) {
            auto Pred = BB.getSinglePredecessor();
            if (!Pred || Pred->getSingleSuccessor() != &BB) {
                continue;
            }
            if (Pred == &BB) {
                continue;
            }
            
            BB.replaceAllUsesWith(Pred);
            for (auto& PN : make_early_inc_range(BB.phis())) {
                PN.replaceAllUsesWith(PN.getIncomingValue(0));
                PN.eraseFromParent();
            }
            for (auto& I : make_early_inc_range(BB)) {
                I.moveBefore(Pred->getTerminator());
            }
            
            Pred->getTerminator()->eraseFromParent();
            BB.eraseFromParent();
            
            found = true;
        }
        if (found) {
            simplify();
        }
    }

}
