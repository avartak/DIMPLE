#include <iostream>
#include <Compile.h>
#include <InputManager.h>
#include <AST.h>
#include <GST.h>
#include <Parser.h>
#include <Translator.h>
#include <Backend.h>
#include <Globals.h>

namespace avl {

    void compile(const std::string& srcfile, const std::string& outfile) {

        TheModule       = std::make_unique<llvm::Module>("AVL module", avl::TheContext);

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
    }

}
