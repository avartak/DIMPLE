#include <llvm/IR/CFG.h>
#include <Function.h>
#include <FunctionType.h>
#include <CodeBlock.h>
#include <Statement.h>
#include <Globals.h>

namespace avl {

    Function::Function(int s, const std::string& n, const std::shared_ptr<Type>& t):
        Instance(VALUE_FUNC, s, n, t),
        lst(std::make_shared<LST>())
    {
        auto ft = static_cast<FunctionType*>(type.get());
        auto linkage = (s == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
        auto fptr = llvm::Function::Create(llvm::cast<llvm::FunctionType>(type->llvm_type), linkage, n, *TheModule);
        if (!ft->ret->retDirectly()) {
            fptr->getArg(0)->addAttr(llvm::Attribute::StructRet);
            fptr->getArg(0)->addAttr(llvm::Attribute::NoAlias);
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            if (!ft->args[i].type->passDirectly()) {
                auto idx = ft->ret->retDirectly() ? i : i+1;
                fptr->getArg(idx)->addAttr(llvm::Attribute::ByVal);
                fptr->getArg(idx)->addAttr(llvm::Attribute::getWithAlignment(TheContext, llvm::Align(8)));
            }
        }
        llvm_pointer = fptr;
    }

    llvm::Value* Function::val() const {
        return llvm_value;
    }

    llvm::Value* Function::ptr() const {
        return llvm_pointer;
    }

	void Function::init() {
        auto fn = llvm::cast<llvm::Function>(llvm_pointer);
        auto ft = static_cast<FunctionType*>(type.get());

        const auto& block = std::make_shared<CodeBlock>(*this);

        if (!ft->ret->retDirectly()) {
            retvar = std::make_shared<Variable>(STORAGE_LOCAL, "", ft->ret);
            retvar->llvm_pointer = fn->getArg(0);
            retblock = std::make_shared<CodeBlock>();
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            auto idx = ft->ret->retDirectly() ? i : i+1;
            auto var = std::make_shared<Variable>(STORAGE_LOCAL, "", ft->args[i].type);
            if (ft->args[i].type->passDirectly()) {
                var->llvm_pointer = TheBuilder.CreateAlloca(var->type->llvm_type);
                var->align();
                if (var->type->isCompound()) {
                    auto u64 = TheBuilder.CreateBitCast(var->llvm_pointer, TheBuilder.getInt64Ty());
                    TheBuilder.CreateStore(fn->getArg(idx), u64);
                }
                else {
                    TheBuilder.CreateStore(fn->getArg(idx), var->ptr());
                }
            }
            else {
                var->llvm_pointer = fn->getArg(idx);
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

    bool Function::checkTerminations() const {

        auto fn = llvm::cast<llvm::Function>(llvm_pointer);
        auto current_block = TheBuilder.GetInsertBlock();
		bool status = true;

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
            else if (retvar && current_block == &*iter) {
                TheBuilder.CreateRet(retvar->val());
            }
			else {
                status = false;
            }
        }

        while (true) {
            std::vector<llvm::BasicBlock*> orphans;
            for (auto iter = fn->getBasicBlockList().begin(); iter != fn->getBasicBlockList().end(); iter++) {
                if (&*iter == &fn->getEntryBlock() || iter->hasNPredecessorsOrMore(1)) {
                    continue;
                }
                orphans.push_back(&*iter);
            }
            if (orphans.size() == 0) {
                break;
            }
            
            for (auto orphan : orphans) {
                for (llvm::BasicBlock* succ : successors(orphan)) {
                    succ->removePredecessor(orphan);
                }
                while (!orphan->empty()) {
                    auto& inst = orphan->back();
                    inst.replaceAllUsesWith(llvm::UndefValue::get(inst.getType()));
                    inst.eraseFromParent();
                }
                orphan->eraseFromParent();
            }
        }

        TheBuilder.SetInsertPoint(current_block);
        return status;
    }
}
