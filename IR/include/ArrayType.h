#ifndef ARRAYTYPE_H
#define ARRAYTYPE_H

#include <memory>
#include <string>
#include <Type.h>

namespace avl {

    struct ArrayType : public Type {

        std::shared_ptr<Type> array_of;
        std::size_t nelements; 

        ArrayType(const std::string&);
        ArrayType(const std::shared_ptr<Type>&, std::size_t);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

    };

}

#endif

