#include <MemoryOp.h>
#include <Globals.h>

namespace avl {

    bool MemoryOp::memset(const std::shared_ptr<Variable>& var, uint8_t v) {
		TheBuilder.CreateMemSet(var->ptr(), TheBuilder.getInt8(v), var->type->size(), llvm::MaybeAlign(0));
		return true;
    }

    bool MemoryOp::memcpy(const std::shared_ptr<Variable>& dest, const std::shared_ptr<Variable>& source) {
        auto sz_src = source->type->size();
        auto sz_dst = dest->type->size();
        if (sz_src != sz_dst) {
            return false;
        }

		TheBuilder.CreateMemCpy(dest->ptr(), llvm::MaybeAlign(0), source->ptr(), llvm::MaybeAlign(0), source->type->size());
		return true;
    }

}
