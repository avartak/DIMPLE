#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <IR/Value.h>

namespace dmp {

    struct Instance : public Value {

        uint16_t storage;
        std::string name;

        Instance(uint16_t, uint16_t, const std::string&, const std::shared_ptr<Type>&);

        virtual ~Instance() = default;

        virtual llvm::Value* val() const override = 0;
        virtual llvm::Value* ptr() const = 0;
        virtual void declare() = 0;
        virtual void init() = 0;
    };

}

#endif
