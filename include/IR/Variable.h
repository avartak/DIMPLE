#ifndef VARIABLE_H
#define VARIABLE_H

#include <IR/Instance.h>
#include <AST/Statement.h>

namespace dmp {

    struct Variable : public Instance {

        Variable(int, const std::string&, const std::shared_ptr<Type>&);

        virtual llvm::Value* val() const override;
        virtual llvm::Value* ptr() const override;
        virtual void declare() override;
        virtual void init() override;

        void initGlobal(const std::shared_ptr<Value>&);
        void initExternal();
        bool align();

        inline bool isGlobal() {
            return (storage == STORAGE_EXTERNAL || storage == STORAGE_INTERNAL);
        }
    };

}

#endif
