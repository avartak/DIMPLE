#include <Globals.h>

namespace dmp {

    std::unique_ptr<llvm::LLVMContext> TheContext;
    std::unique_ptr<llvm::Module> TheModule;
    std::unique_ptr<llvm::IRBuilder<> > TheBuilder;

}
