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
	
	void CodeBlock::insert(const std::shared_ptr<CodeBlock>& prior_block) {
		block->insertInto(prior_block->block->getParent()); 
		block->moveAfter(prior_block->block); 
		TheBuilder.SetInsertPoint(block);
	}
	
	void CodeBlock::insert() {
		auto b = TheBuilder.GetInsertBlock();
		block->insertInto(b->getParent()); 
		block->moveAfter(b); 
		TheBuilder.SetInsertPoint(block);
	}

	void CodeBlock::jump() {
		if (TheBuilder.GetInsertBlock()->getTerminator() == nullptr) {
            TheBuilder.CreateBr(block);
        }
	}
	
	void CodeBlock::branch(const std::shared_ptr<Value>& ex, const std::shared_ptr<CodeBlock>& alt) {
		if (TheBuilder.GetInsertBlock()->getTerminator() == nullptr) {
            TheBuilder.CreateCondBr(ex->val(), block, alt->block); 
        }
	}

}
