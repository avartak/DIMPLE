#ifndef BINARYOP_H
#define BINARYOP_H

#include <memory>
#include <Value.h>
#include <Variable.h>
#include <Literal.h>

namespace avl {

    struct BinaryOp {

        static std::shared_ptr<Value> recast(const std::shared_ptr<Value>&, const std::shared_ptr<Type>&);
        static std::shared_ptr<Value> recastImplicit(const std::shared_ptr<Value>&, const std::shared_ptr<Type>&);
        static std::shared_ptr<Value> add(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> subtract(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> multiply(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> divide(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> remainder(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> equal(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> unequal(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> greater(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> lesser(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> greaterEqual(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> lesserEqual(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> shiftLeft(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> shiftRight(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> bitAnd(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> bitXor(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> bitOr(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> logAnd(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> logOr(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> element(const std::shared_ptr<Variable>&, const std::shared_ptr<Value>&);
        static std::shared_ptr<Value> member(const std::shared_ptr<Variable>&, const std::string&);
        static std::shared_ptr<Value> assign(const std::shared_ptr<Variable>&, const std::shared_ptr<Value>&);
        static bool isValidDivision(const std::shared_ptr<Value>&, const std::shared_ptr<Value>&);

    };

}

#endif
