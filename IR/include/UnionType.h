#ifndef UNIONTYPE_H
#define UNIONTYPE_H

#include <vector>
#include <string>
#include <Type.h>
#include <Value.h>
#include <NameType.h>

namespace dmp {

    class UnionType : public Type {

        public:
        std::vector<NameType> members;

        UnionType(const std::string&);
        UnionType(const std::vector<NameType>&);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

        static std::shared_ptr<Value> initConst(const std::shared_ptr<UnionType>&, const std::shared_ptr<Value>&);

    };

}

#endif

