#include <Variable.h>
#include <PointerType.h>
#include <MemoryOp.h>
#include <Globals.h>

namespace avl {

    Variable::Variable(int s, const std::string& n, const std::shared_ptr<Type>& t):
        Instance(VALUE_VAR, s, n, t)
    {
    }

    llvm::Value* Variable::val() const {
        if (llvm_value != nullptr) {
            return TheBuilder.CreateAlignedLoad(type->llvm_type, ptr(), llvm::MaybeAlign(type->alignment()));
        }
        return nullptr;
    }

    llvm::Value* Variable::ptr() const {
        auto t = std::make_shared<PointerType>(type);
        if (llvm_value == nullptr) {
            return llvm_value;
        }
        else if (llvm::isa<llvm::GlobalVariable>(llvm_value)) {
            if (llvm::cast<llvm::GlobalVariable>(llvm_value)->getType()->getElementType() != type->llvm_type) {
                return TheBuilder.CreateBitCast(llvm_value, t->llvm_type);
            }
        }
        else if (llvm::isa<llvm::AllocaInst>(llvm_value)) {
            if (llvm::cast<llvm::AllocaInst>(llvm_value)->getType()->getElementType() != type->llvm_type) {
                return TheBuilder.CreateBitCast(llvm_value, t->llvm_type);
            }
        }
        return llvm_value;
    }

    void Variable::declare() {
        if (isGlobal()) {
            auto linkage = (storage == STORAGE_EXTERNAL ? llvm::GlobalVariable::ExternalLinkage : llvm::GlobalVariable::InternalLinkage);
            llvm_value = new llvm::GlobalVariable(*TheModule, type->llvm_type, false, linkage, nullptr, name);
        }
        else {
            auto current_insert_pt = TheBuilder.saveIP();
            TheBuilder.SetInsertPoint(TheBuilder.GetInsertBlock()->getParent()->getEntryBlock().getTerminator());
            llvm_value = TheBuilder.CreateAlloca(type->llvm_type);
            TheBuilder.restoreIP(current_insert_pt);
        }
        align();
    }

    void Variable::init() {
        if (isGlobal()) {
            llvm::cast<llvm::GlobalVariable>(llvm_value)->setInitializer(llvm::Constant::getNullValue(type->llvm_type));
        }
        else {
            if (!type->moveDirectly()) {
                MemoryOp::memset(this, 0);
            }
            else {
                TheBuilder.CreateStore(llvm::Constant::getNullValue(type->llvm_type), ptr());
            }
        }
    }

    void Variable::initGlobal(const std::shared_ptr<Value>& init) {
        if (isGlobal() && llvm_value != nullptr && (*init->type == *type) && init->isConst()) {
            llvm::cast<llvm::GlobalVariable>(llvm_value)->setInitializer(llvm::cast<llvm::Constant>(init->val()));
        }
    }

    void Variable::initExternal() {
        if (isGlobal() && llvm_value != nullptr) {
            llvm::cast<llvm::GlobalVariable>(llvm_value)->setExternallyInitialized(true);
        }
    }

    bool Variable::align() {
        std::size_t al = type->alignment();
        if (al == 0 || llvm_value == nullptr) {
            return false;
        }
        if (llvm::isa<llvm::AllocaInst>(llvm_value)) {
            auto v = llvm::cast<llvm::AllocaInst>(llvm_value);
            v->setAlignment(llvm::Align(al));
        }
        else {
            auto v = llvm::cast<llvm::GlobalVariable>(llvm_value);
            v->setAlignment(llvm::Align(al));
        }
        return true;
	}
}
