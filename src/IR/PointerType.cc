#include <llvm/IR/DerivedTypes.h>
#include <IR/PointerType.h>

namespace dmp {

    PointerType::PointerType(const std::string& n):
        Type(TYPE_POINTER, n)
    {
    }

    PointerType::PointerType(const std::shared_ptr<Type>& pt):
        Type(TYPE_POINTER),
        points_to(pt)
    {
        llvm_type = llvm::PointerType::get(pt->llvm_type, 0);
        complete = true;
    }

    bool PointerType::operator==(const Type& t) const {
        if (is != t.is) return false;
        for (auto bc : being_compared) {
            if (bc.first == this && bc.second == &t) {
                return true;
            }
        }

        being_compared.push_back(std::pair<const Type*, const Type*>(this, &t));
        auto pt = static_cast<const PointerType*>(&t);
        bool retval = (*points_to == *pt->points_to);
        being_compared.pop_back();
        return retval;
    }

    bool PointerType::construct(const std::shared_ptr<Type>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto pt = static_cast<const PointerType*>(t.get());
        points_to = pt->points_to;
        llvm::cast<llvm::StructType>(llvm_type)->setBody(pt->llvm_type, true);
        complete = true;
        return true;
    }

    std::shared_ptr<Type> PointerType::clone() const {
        auto pt = std::make_shared<PointerType>(points_to); // Cloning points_to may cause a feedback loop
        pt->attr = attr;
        pt->name = name;
        pt->llvm_type = llvm_type;
        pt->complete = complete;
        return pt;
    }

}

