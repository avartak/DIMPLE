#ifndef UNKNOWNTYPE_H
#define UNKNOWNTYPE_H

#include <Type.h>

namespace dmp {

    struct UnknownType : public Type {

        UnknownType();

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

    };

}

#endif

