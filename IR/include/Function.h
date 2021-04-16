#ifndef FUNCTION_H
#define FUNCTION_H

#include <memory>
#include <vector>
#include <llvm/IR/BasicBlock.h>
#include <Instance.h>
#include <Variable.h>

namespace avl {

    struct CodeBlock;
    struct LST;

    struct Function : public Instance {

        std::shared_ptr<LST> lst;
        std::vector<std::shared_ptr<Variable> > args;
        std::shared_ptr<Variable> retvar;
        std::shared_ptr<CodeBlock> retblock;
        llvm::BasicBlock* freeze_block;

        Function(int, const std::string&, const std::shared_ptr<Type>&);

        virtual llvm::Value* val() const override;
        virtual llvm::Value* ptr() const override;
        virtual void declare() override;
        virtual void init() override;

        void descope();
        void freeze();
        bool resume();
        bool checkTerminations();
        void simplify();
    };

}

#endif
