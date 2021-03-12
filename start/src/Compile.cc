#include <iostream>
#include <Compile.h>
#include <AST.h>
#include <Parser.h>
/*
#include <Expander.h>
#include <Analyzer.h>
*/
#include <Backend.h>
#include <Globals.h>

namespace avl {

    void compile(const std::string& srcfile, const std::string& outfile) {

        auto ast = std::make_shared<AST>();
        auto parser = std::make_shared<Parser>(ast);
        /*
        auto expander = std::make_shared<Expander>(ast);
        auto analyzer = std::make_shared<Analyzer>(ast);
        */
        auto backend = std::make_shared<Backend>(srcfile, outfile);

        TheInput->set(srcfile);
        if (!TheInput->isValid()) {
            std::cerr << "Unable to open source file " + srcfile;
        }

        if (!parser->run()) {
            std::cerr << parser->errorPrintout();
            return;
        }

        /*
        if (!expander->run()) {
            std::cerr << expander->errorPrintout();
            return;
        }

        if (!analyzer->run()) {
            std::cerr << analyzer->errorPrintout();
            return;
        }
        */

        if (!backend->run()) {
            std::cerr << backend->errorPrintout();
            return;
        }
    }

}
