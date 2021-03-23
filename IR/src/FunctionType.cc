#include <FunctionType.h>
#include <Globals.h>

namespace avl {

    FunctionType::FunctionType(const std::vector<NameType>& a, const std::shared_ptr<Type>& r):
        Type(TYPE_FUNCTION),
        args(a),
        ret(r)
    {
        std::vector<llvm::Type*> tv;
        if (!ret->retDirectly()) {
            tv.push_back(ret->llvm_type);
        }
        for (const auto& a : args) {
            if (a.type->passDirectly()) {
                if (a.type->isCompound()) {
				    tv.push_back(TheBuilder.getInt64Ty());
                }
                else {
                    tv.push_back(a.type->llvm_type);
                }
            }
            else {
                tv.push_back(llvm::PointerType::get(a.type->llvm_type, 0));
            }
        }
        llvm::Type* ret_lt;
        if (!ret->retDirectly()) {
            ret_lt = TheBuilder.getVoidTy();
        }
        else {
            if (ret->isCompound()) {
                ret_lt = TheBuilder.getInt64Ty();
            }
            else {
                ret_lt = ret->llvm_type;
            }
        }
        llvm_type = llvm::FunctionType::get(ret_lt, tv, false);
        complete = true;
    }

    bool FunctionType::operator==(const Type& t) const {
        if (t.is != is) return false;
        for (const auto& bc : being_compared) {
            if (bc.first == this && bc.second == &t) {
                return true;
            }
        }

        being_compared.push_back(std::pair<const Type*, const Type*>(this, &t));
        auto ft = static_cast<const FunctionType*>(&t);
        if (args.size() != ft->args.size()) {
            being_compared.pop_back();
            return false;
        }
        for (std::size_t i = 0; i < args.size(); i++) {
            if (*args[i].type != *ft->args[i].type) {
                being_compared.pop_back();
                return false;
            }
        }
        if (*ret != *ft->ret) {
            being_compared.pop_back();
            return false;
        }
        being_compared.pop_back();
        return true;
    }

    bool FunctionType::construct(const std::shared_ptr<Type>& t) {
        return false;
    }

    std::shared_ptr<Type> FunctionType::clone() const {
        std::vector<NameType> ntv;
        for (const auto& a : args) {
            ntv.push_back(NameType(a.name, a.type->clone()));
        }
        auto ft = std::make_shared<FunctionType>(ntv, ret->clone());
        ft->flags = flags;
        ft->name = name;
        ft->llvm_type = llvm_type;
        ft->complete = complete;
        return ft;
    }
}
