#include <llvm/IR/Instructions.h>
#include <llvm/IR/GlobalVariable.h>
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
        if (llvm_pointer != nullptr && llvm_value == nullptr) {
            return TheBuilder.CreateAlignedLoad(type->llvm_type, ptr(), llvm::MaybeAlign(type->alignment()));
        }
        return llvm_value;
    }

    llvm::Value* Variable::ptr() const {
        if (llvm::isa<llvm::GlobalVariable>(llvm_pointer)) {
            if (llvm::cast<llvm::GlobalVariable>(llvm_pointer)->getType()->getElementType() != type->llvm_type) {
                auto t = std::make_shared<PointerType>(type);
                return TheBuilder.CreateBitCast(llvm_pointer, t->llvm_type);
            }
        }
        else if (llvm::isa<llvm::AllocaInst>(llvm_pointer)) {
            if (llvm::cast<llvm::AllocaInst>(llvm_pointer)->getType()->getElementType() != type->llvm_type) {
                auto t = std::make_shared<PointerType>(type);
                return TheBuilder.CreateBitCast(llvm_pointer, t->llvm_type);
            }
        }
        return llvm_pointer;
    }

    void Variable::init() {
        if (!type->moveDirectly()) {
            MemoryOp::memset(this, 0);
        }
        else {
            TheBuilder.CreateStore(llvm::Constant::getNullValue(type->llvm_type), ptr());
        }
    }

    bool Variable::align() {
        std::size_t al = type->alignment();
        
        if (al == 0 || llvm_pointer == nullptr) {
            return false;
        }
        
        if (llvm::isa<llvm::AllocaInst>(llvm_pointer)) {
            auto v = llvm::cast<llvm::AllocaInst>(llvm_pointer);
            v->setAlignment(llvm::Align(al));
        }
        else {
            auto v = llvm::cast<llvm::GlobalVariable>(llvm_pointer);
            v->setAlignment(llvm::Align(al));
        }
        
        return true;
	}

}