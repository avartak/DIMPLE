#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include <llvm/Target/TargetMachine.h>
#include <Pass.h>

namespace dmp {

    struct Backend : public Pass<void> {

        llvm::TargetMachine* machine;
        std::string srcfile;
        std::string outfile;

        Backend(const std::string&, const std::string&);

        virtual bool run() override;
    };

}

#endif
