#ifndef ARRAYTYPE_H
#define ARRAYTYPE_H

#include <map>
#include <memory>
#include <string>
#include <IR/Type.h>
#include <IR/Value.h>

namespace dmp {

    struct ArrayType : public Type {

        std::shared_ptr<Type> array_of;
        std::size_t nelements; 

        ArrayType(const std::string&);
        ArrayType(const std::shared_ptr<Type>&, std::size_t);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

        static std::shared_ptr<Value> initConst(const std::shared_ptr<ArrayType>&, const std::map<std::size_t, std::shared_ptr<Value> >&);

    };

}

#endif

