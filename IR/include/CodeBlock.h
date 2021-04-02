#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <memory>
#include <llvm/IR/BasicBlock.h>
#include <Value.h>
#include <Function.h>

namespace avl {

    struct CodeBlock {

        llvm::BasicBlock* block;
    
        CodeBlock();
        CodeBlock(llvm::BasicBlock*);
        CodeBlock(const Function&);
        
        void insert(const std::shared_ptr<CodeBlock>&);
        void insert();
        void jump();
        void branch(const std::shared_ptr<Value>&, const std::shared_ptr<CodeBlock>&);					
    	
    };

}

#endif
