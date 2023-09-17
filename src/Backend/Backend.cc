#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>

#include <common/Globals.h>
#include <Backend/Backend.h>

namespace dmp {

    Backend::Backend(const std::string& src, const std::string& out): 
        Pass(nullptr, nullptr, nullptr),
        srcfile(src),
        outfile(out)
    {
    
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();
        
        auto triple = llvm::sys::getDefaultTargetTriple();
        std::string error_msg;
        auto target = llvm::TargetRegistry::lookupTarget(triple, error_msg);

        if (!target) {
            error(error_msg);
        }
        else {
            auto CPU = llvm::sys::getHostCPUName();
            auto features = "";
            llvm::TargetOptions options;
            auto reloc_model = std::optional<llvm::Reloc::Model>();
            
            machine = target->createTargetMachine(triple, CPU, features, options, reloc_model);

            if (machine == nullptr) {
                error("Unable to detect target machine");
            }
            else {
                TheModule->setSourceFileName(srcfile);
                TheModule->setTargetTriple(triple);
                TheModule->setDataLayout(machine->createDataLayout());
            }
        }
    }

    bool Backend::run() {

        if (hasErrors()) {
            return error();
        }

        TheModule->print(llvm::outs(), nullptr);

        auto outfiletype = llvm::CGFT_ObjectFile;
    
        std::error_code EC;
        llvm::raw_fd_ostream dest(outfile, EC, llvm::sys::fs::OF_None);
        if (EC) {
            return error("Unable to open " + outfile + "; " + EC.message());
        }        

        llvm::legacy::PassManager pass;
        if (machine->addPassesToEmitFile(pass, dest, nullptr, outfiletype)) {
            return error("Unable to produce object file");
        }
        
        pass.run(*TheModule);
        dest.flush();
        return success();
    }

}
