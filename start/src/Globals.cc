#include <Globals.h>

namespace avl {

    llvm::LLVMContext TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    llvm::IRBuilder<> TheBuilder(TheContext);

}
