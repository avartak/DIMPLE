#ifndef GLOBALS_H
#define GLOBALS_H

#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace avl {

    extern std::unique_ptr<llvm::LLVMContext> TheContext;
    extern std::unique_ptr<llvm::Module> TheModule;
    extern std::unique_ptr<llvm::IRBuilder<> > TheBuilder;

}

#endif
