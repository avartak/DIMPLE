#include <Globals.h>

namespace avl {

    llvm::LLVMContext TheContext;
    std::unique_ptr<llvm::Module> TheModule = std::make_unique<llvm::Module>("AVL module", TheContext);
    llvm::IRBuilder<> TheBuilder(TheContext);
    std::unique_ptr<InputManager> TheInput = std::make_unique<InputManager>();

}
