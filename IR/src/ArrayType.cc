#include <llvm/IR/DerivedTypes.h>
#include <ArrayType.h>
#include <Globals.h>

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

    std::shared_ptr<Value> ArrayType::initConst(const std::shared_ptr<ArrayType>& t, const std::map<std::size_t, std::shared_ptr<Value> >& cmap) {

        std::vector<std::pair<std::size_t, std::shared_ptr<Value> > > cvec;

        for (const auto& icmap : cmap) {
            cvec.push_back(icmap);
        }

        struct {
            bool operator()(std::pair<std::size_t, std::shared_ptr<Value> > a, std::pair<std::size_t, std::shared_ptr<Value> > b) const {
                return a.first < b.first;
            }
        } sorter;

        std::sort(cvec.begin(), cvec.end(), sorter);

        std::vector<llvm::Type*> tv;
        std::vector<llvm::Constant*> cv;
        for (std::size_t i = 0; i < cvec.size(); i++) {
            if (i == 0 && cvec[i].first > 0) {
                tv.push_back(llvm::ArrayType::get(t->array_of->llvm_type, cvec[i].first));
                cv.push_back(llvm::Constant::getNullValue(tv.back()));
            }
            tv.push_back(cvec[i].second->type->llvm_type);
            cv.push_back(llvm::cast<llvm::Constant>(cvec[i].second->llvm_value));
            if (i < cvec.size()-1) {
                if (cvec[i+1].first-1-cvec[i].first > 0) {
                    tv.push_back(llvm::ArrayType::get(t->array_of->llvm_type, cvec[i+1].first-1-cvec[i].first));
                    cv.push_back(llvm::Constant::getNullValue(tv.back()));
                }
            }
            else if (cvec[i].first < t->nelements-1) {
                tv.push_back(llvm::ArrayType::get(t->array_of->llvm_type, t->nelements-1-cvec[i].first));
                cv.push_back(llvm::Constant::getNullValue(tv.back()));
            }
        }

        t->llvm_type = llvm::StructType::get(TheContext, tv, true);
        return std::make_shared<Value>(t, llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(t->llvm_type), cv));
    }
}

