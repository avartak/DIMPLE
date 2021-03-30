#ifndef STRUCTTYPE_H
#define STRUCTTYPE_H

#include <vector>
#include <string>
#include <Type.h>
#include <NameType.h>

namespace avl {

    class StructType : public Type {

        public:
        std::vector<NameType> members;

        StructType(const std::string&, bool);
        StructType(const std::vector<NameType>&, bool);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

        inline bool isPacked() const {
            return (flags & 1) > 0;
        }
    };

}

#endif
