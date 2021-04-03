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
        
        static void insert(const std::shared_ptr<CodeBlock>&);
        static void jump(const std::shared_ptr<CodeBlock>&);
        static void branch(const std::shared_ptr<Value>&, const std::shared_ptr<CodeBlock>&, const std::shared_ptr<CodeBlock>&);					
    	
    };

}

#endif
