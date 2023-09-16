#include <llvm/IR/DerivedTypes.h>
#include <Value.h>
#include <PointerType.h>

namespace dmp {

    Value::Value(uint16_t i, const std::shared_ptr<Type>& t, llvm::Value* v):
        Entity(ENTITY_VALUE),
        is(i),
        llvm_value(v)
    {
        setType(t);
    }

    Value::Value(const std::shared_ptr<Type>& t, llvm::Value* v):
        Entity(ENTITY_VALUE),
        is(VALUE_VAL),
        llvm_value(v)
    {
        setType(t);
    }

    llvm::Value* Value::val() const {
        return llvm_value;
    }

    void Value::setType(const std::shared_ptr<Type>& t) {
        if (t->isPtr() && llvm::isa<llvm::StructType>(t->llvm_type)) {
            auto pt = static_cast<PointerType*>(t.get());
            type = t->clone();
            type->llvm_type = llvm::PointerType::get(pt->points_to->llvm_type, 0);
        }
        else {
            type = t;
        }
    }
}
