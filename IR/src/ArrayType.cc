#include <llvm/IR/DerivedTypes.h>
#include <ArrayType.h>

namespace avl {

    ArrayType::ArrayType(const std::string& n):
        Type(TYPE_ARRAY, n)
    {
    }

    ArrayType::ArrayType(const std::shared_ptr<Type>& ao, std::size_t n):
        Type(TYPE_ARRAY),
        array_of(ao),
        nelements(n)
    {
        llvm_type = llvm::ArrayType::get(ao->llvm_type, n);
        complete = true;
    }

    bool ArrayType::operator==(const Type& t) const {
        if (is != t.is) return false;
        for (auto bc : being_compared) {
            if (bc.first == this && bc.second == &t) {
                return true;
            }
        }

        being_compared.push_back(std::pair<const Type*, const Type*>(this, &t));
        auto at = static_cast<const ArrayType*>(&t);
        bool retval = (nelements == at->nelements) && (*array_of == *at->array_of);
        being_compared.pop_back();
        return retval;
    }

    bool ArrayType::construct(const std::shared_ptr<Type>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto at = static_cast<const ArrayType*>(t.get());
        array_of = at->array_of;
        nelements = at->nelements;
        llvm::cast<llvm::StructType>(llvm_type)->setBody(at->llvm_type, true);
        complete = true;
        return true;
    }
 
    std::shared_ptr<Type> ArrayType::clone() const {
        auto at = std::make_shared<ArrayType>(array_of->clone(), nelements);
        at->flags = flags;
        at->name = name;
        at->llvm_type = llvm_type;
        at->complete = complete;
        return at;
    }

}

