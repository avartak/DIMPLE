#ifndef STRUCTTYPE_H
#define STRUCTTYPE_H

#include <vector>
#include <map>
#include <string>
#include <IR/Type.h>
#include <IR/Value.h>
#include <IR/NameType.h>

namespace dmp {

    class StructType : public Type {

        public:
        std::vector<NameType> members;

        StructType(const std::string&, bool);
        StructType(const std::vector<NameType>&, bool);

        virtual bool operator==(const Type&) const override;
        virtual bool construct(const std::shared_ptr<Type>&) override;
        virtual std::shared_ptr<Type> clone() const override;

        inline bool isPacked() const {
            return (attr & 1) > 0;
        }

        static std::shared_ptr<Value> initConst(const std::shared_ptr<StructType>&, const std::map<std::size_t, std::shared_ptr<Value> >&);

    };

}

#endif

