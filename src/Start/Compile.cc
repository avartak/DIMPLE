#include <iostream>
#include <Common/Globals.h>
#include <Start/Compile.h>
#include <IO/InputManager.h>
#include <AST/AST.h>
#include <IR/GST.h>
#include <Parser/Parser.h>
#include <Translator/Translator.h>
#include <Backend/Backend.h>

namespace dmp {

    void compile(const std::string& srcfile, const std::string& outfile) {

        TheContext      = std::make_unique<llvm::LLVMContext>();
        TheModule       = std::make_unique<llvm::Module>("DIMPLE module", *TheContext);
        TheBuilder      = std::make_unique<llvm::IRBuilder<> >(*TheContext);

        auto input      = std::make_unique<InputManager>();
        auto ast        = std::make_unique<AST>();
        auto gst        = std::make_unique<GST>();

        auto parser     = std::make_unique<Parser>(input.get(), ast.get());
        auto translator = std::make_unique<Translator>(input.get(), ast.get(), gst.get());
        auto backend    = std::make_unique<Backend>(srcfile, outfile);

        input->set(srcfile);
        if (!input->isValid()) {
            std::cerr << "Unable to open source file " + srcfile;
        }

	else if (!parser->run()) {
            std::cerr << parser->errorPrintout();
        }

	else if (!translator->run()) {
            std::cerr << translator->errorPrintout();
        }

	else if (!backend->run()) {
            std::cerr << backend->errorPrintout();
        }

	TheModule.reset();
	TheContext.reset();
	TheBuilder.reset();
    }

}
