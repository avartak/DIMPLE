#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>
#include <vector>
#include <Instance.h>
#include <LST.h>
#include <Variable.h>

namespace avl {

    struct CodeBlock;

    struct Function : public Instance {

        std::shared_ptr<LST> lst;
        std::vector<std::shared_ptr<Variable> > args;
        std::shared_ptr<Variable> retvar;
        std::shared_ptr<CodeBlock> retblock;

        Function(int, const std::string&, const std::shared_ptr<Type>&);

        virtual llvm::Value* val() const override;
        virtual llvm::Value* ptr() const override;
        virtual void init() override;

        void resetLocals();
        bool checkTerminations() const;
        void simplify();
    };

}

#endif
