#ifndef UNIONTYPE_H
#define UNIONTYPE_H

#include <vector>
#include <string>
#include <Type.h>
#include <NameType.h>

namespace avl {

    class UnionType : public Type {

        public:
        std::vector<NameType> members;

        UnionType(const std::string&);
        UnionType(const std::vector<NameType>&);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;
    };

}

#endif

