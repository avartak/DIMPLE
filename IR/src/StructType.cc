#include <StructType.h>
#include <Globals.h>

namespace avl {

    StructType::StructType(const std::string& n, bool p):
        Type(TYPE_STRUCT, n)
    {
        if (p) {
            flags |= 1;
        }
    }

    StructType::StructType(const std::vector<NameType>& m, bool p):
        Type(TYPE_STRUCT),
        members(m)
    {
        if (p) {
            flags |= 1;
        }
        std::vector<llvm::Type*> tv;
        for (const auto& im : m) {
            tv.push_back(im.type->llvm_type);
        }
        llvm_type = llvm::StructType::get(TheContext, tv, p);
        complete = true;
    }

    bool StructType::operator==(const Type& t) const {
        if (t.is != is) return false;
        for (const auto& bc : being_compared) {
            if (bc.first == this && bc.second == &t) {
                return true;
            }
        }

        being_compared.push_back(std::pair<const Type*, const Type*>(this, &t));
        auto st = static_cast<const StructType*>(&t);
        if (isPacked() != st->isPacked()) {
            being_compared.pop_back();
            return false;
        }
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

    bool StructType::construct(const std::shared_ptr<Type>& t) {
        if (isComplete() || t->is != is) {
            return false;
        }
        auto st = static_cast<const StructType*>(t.get());
        if (st->isPacked() != isPacked()) {
            return false;
        }
        members = st->members;
        auto this_lt = llvm::cast<llvm::StructType>(llvm_type);
        auto that_lt = llvm::cast<llvm::StructType>(st->llvm_type);
        this_lt->setBody(that_lt->elements(), isPacked());
        complete = true;
        return true;
    }

    std::shared_ptr<Type> StructType::clone() const {
        std::vector<NameType> ntv;
        for (const auto& m : members) {
            ntv.push_back(NameType(m.name, m.type->clone()));
        }
        auto st = std::make_shared<StructType>(ntv, isPacked());
        st->flags = flags;
        st->name = name;
        st->llvm_type = llvm_type;
        st->complete = complete;
        return st;
    }

}
