#include <UnionType.h>
#include <Globals.h>

namespace avl {

    UnionType::UnionType(const std::string& n):
        Type(TYPE_UNION, n)
    {
    }

    UnionType::UnionType(const std::vector<NameType>& m):
        Type(TYPE_STRUCT),
        members(m)
    {
        std::size_t sz_max = 0;
        auto imax = *members.begin();
        for (const auto& im : members) {
            auto isz = im.type->size();
            if (isz > sz_max) {
                sz_max = isz;
                imax = im;
            }
        }

        std::vector<llvm::Type*> tv;
        tv.push_back(imax.type->llvm_type);
        llvm_type = llvm::StructType::get(TheContext, tv, true);
        complete = true;
    }

    bool UnionType::operator==(const Type& t) const {
        if (t.is != is) return false;
        for (const auto& bc : being_compared) {
            if (bc.first == this && bc.second == &t) {
                return true;
            }
        }

        being_compared.push_back(std::pair<const Type*, const Type*>(this, &t));
        auto st = static_cast<const UnionType*>(&t);
        if (members.size() != st->members.size()) {
            being_compared.pop_back();
            return false;
        }
        for (std::size_t i = 0; i < members.size(); i++) {
            if (members[i].name->name != st->members[i].name->name) {
                being_compared.pop_back();
                return false;
            }
            if (*members[i].type != *st->members[i].type) {
                being_compared.pop_back();
                return false;
            }
        }
        being_compared.pop_back();
        return true;
    }

    bool UnionType::construct(const std::shared_ptr<Type>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto ut = static_cast<const UnionType*>(t.get());
        members = ut->members;
        auto this_lt = llvm::cast<llvm::StructType>(llvm_type);
        auto that_lt = llvm::cast<llvm::StructType>(ut->llvm_type);
        this_lt->setBody(that_lt->elements(), true);
        complete = true;
        return true;
    }

    std::shared_ptr<Type> UnionType::clone() const {
        std::vector<NameType> ntv;
        for (const auto& m : members) {
            ntv.push_back(NameType(m.name, m.type->clone()));
        }
        auto ut = std::make_shared<UnionType>(ntv);
        ut->flags = flags;
        ut->name = name;
        ut->llvm_type = llvm_type;
        ut->complete = complete;
        return ut;
    }
}
