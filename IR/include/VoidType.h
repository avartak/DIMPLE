#ifndef VOIDTYPE_H
#define VOIDTYPE_H

#include <Type.h>

namespace dmp {

    struct VoidType : public Type {

        VoidType();

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

    };

}

#endif

