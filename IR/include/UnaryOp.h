#ifndef UNARYOP_H
#define UNARYOP_H

#include <memory>
#include <Value.h>
#include <Type.h>

namespace avl {

    struct UnaryOp {

        static std::shared_ptr<Value> plus(const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> negate(const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> logicalNot(const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> complement(const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> size(const std::shared_ptr<Type>&);
        static std::shared_ptr<Value> address(const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> dereference(const std::shared_ptr<Value>&);

    };

}

#endif
