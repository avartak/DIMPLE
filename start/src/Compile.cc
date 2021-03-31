#include <iostream>
#include <Compile.h>
#include <InputManager.h>
#include <AST.h>
#include <GST.h>
#include <Parser.h>
#include <Analyzer.h>
#include <Backend.h>
#include <Globals.h>

namespace avl {

    void compile(const std::string& srcfile, const std::string& outfile) {

        auto input = std::make_shared<InputManager>();
        auto ast = std::make_shared<AST>();
        auto gst = std::make_shared<GST>();
        auto parser = std::make_shared<Parser>(input, ast);
        auto analyzer = std::make_shared<Analyzer>(ast, gst);
        auto backend = std::make_shared<Backend>(srcfile, outfile);

        input->set(srcfile);
        if (!input->isValid()) {
            std::cerr << "Unable to open source file " + srcfile;
        }

        if (!parser->run()) {
            std::cerr << parser->errorPrintout();
            return;
        }

        if (!analyzer->run()) {
            std::cerr << analyzer->errorPrintout();
            return;
        }

        if (!backend->run()) {
            std::cerr << backend->errorPrintout();
            return;
        }
    }

}
