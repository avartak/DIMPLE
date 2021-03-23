#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <Value.h>

namespace avl {

    enum Storage_ID {

        STORAGE_UNDEFINED,
        STORAGE_EXTERNAL,
        STORAGE_INTERNAL,
        STORAGE_STATIC,
        STORAGE_LOCAL,
        STORAGE_PRIVATE

    };

    struct Instance : public Value {

        uint16_t storage;
        std::string name;
        llvm::Value* llvm_pointer;

        Instance(uint16_t, uint16_t, const std::string&, const std::shared_ptr<Type>&);

        virtual ~Instance() = default;

        virtual llvm::Value* val() const override = 0;
        virtual llvm::Value* ptr() const = 0;
    };

}

#endif
