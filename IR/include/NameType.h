#ifndef NAMETYPE_H
#define NAMETYPE_H

#include <memory>
#include <Identifier.h>
#include <Type.h>

namespace avl {

    struct NameType {

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Type> type;

        NameType();
        NameType(const std::shared_ptr<Type>&);
        NameType(const std::shared_ptr<Identifier>&, const std::shared_ptr<Type>&);

    };

}

#endif
