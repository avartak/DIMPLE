#include <CodeBlock.h>
#include <Globals.h>

namespace avl {

    CodeBlock::CodeBlock() {
        block = llvm::BasicBlock::Create(TheContext);
    }
    
    CodeBlock::CodeBlock(const Function& func) {
        block = llvm::BasicBlock::Create(TheContext, "", llvm::cast<llvm::Function>(func.llvm_pointer));
        TheBuilder.SetInsertPoint(block);
    }
    
    void CodeBlock::insert(const std::shared_ptr<CodeBlock>& b) {
        auto ib = TheBuilder.GetInsertBlock();
        b->block->insertInto(ib->getParent()); 
        b->block->moveAfter(ib); 
        TheBuilder.SetInsertPoint(b->block);
    }
    
    void CodeBlock::jump(const std::shared_ptr<CodeBlock>& b) {
        if (TheBuilder.GetInsertBlock()->getTerminator() == nullptr) {
            TheBuilder.CreateBr(b->block);
        }
    }
    
    void CodeBlock::branch(const std::shared_ptr<Value>& ex, const std::shared_ptr<CodeBlock>& yes, const std::shared_ptr<CodeBlock>& no) {
        if (TheBuilder.GetInsertBlock()->getTerminator() == nullptr) {
            TheBuilder.CreateCondBr(ex->val(), yes->block, no->block); 
        }
    }

}
