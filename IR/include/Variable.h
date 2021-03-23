#ifndef VARIABLE_H
#define VARIABLE_H

#include <Instance.h>

namespace avl {

    struct Variable : public Instance {

        Variable(int, const std::string&, const std::shared_ptr<Type>&);

        virtual llvm::Value* val() const override;
        virtual llvm::Value* ptr() const override;

        bool align();

    };

}

#endif
