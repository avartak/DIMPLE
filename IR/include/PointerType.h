#ifndef POINTERTYPE_H
#define POINTERTYPE_H

#include <memory>
#include <Type.h>

namespace avl {

    struct PointerType : public Type {

        std::shared_ptr<Type> points_to;

        PointerType(const std::string&);
        PointerType(const std::shared_ptr<Type>&);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

    };

}

#endif

