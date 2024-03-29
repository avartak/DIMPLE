#ifndef FUNCTIONTYPE_H
#define FUNCTIONTYPE_H

#include <vector>
#include <string>
#include <IR/Type.h>
#include <IR/NameType.h>

namespace dmp {

    class FunctionType : public Type {

        public:
        std::vector<NameType> args;
        std::shared_ptr<Type> ret;

        FunctionType(const std::vector<NameType>&, const std::shared_ptr<Type>&);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;
    };

}

#endif

