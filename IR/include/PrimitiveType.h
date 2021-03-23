#ifndef PRIMITIVETYPE_H
#define PRIMITIVETYPE_H

#include <Type.h>

namespace avl {

    struct PrimitiveType : public Type {

        PrimitiveType(int);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

    };

}

#endif

