#ifndef NAMETYPE_H
#define NAMETYPE_H

#include <memory>
#include <AST/Identifier.h>
#include <AST/TypeNode.h>
#include <IR/Type.h>

namespace dmp {

    struct NameType {

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Type> type;
        uint64_t attr;

        NameType();
        NameType(const std::shared_ptr<Type>&);
        NameType(const std::shared_ptr<Type>&, uint64_t);
        NameType(const std::shared_ptr<Identifier>&, const std::shared_ptr<Type>&);
        NameType(const std::shared_ptr<Identifier>&, const std::shared_ptr<Type>&, uint64_t);

        inline bool passByRef() const {
            return (attr & PASS_BY_REFERENCE) != 0;
        }
    };

}

#endif
