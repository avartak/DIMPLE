#ifndef PASS_H
#define PASS_H

#include <memory>
#include <AST.h>
#include <Error.h>
#include <Node.h>

namespace avl {

    template<typename T>
    struct Pass {

        std::shared_ptr<AST> ast;
        std::shared_ptr<T> result;
        std::vector<Error> errors;

        Pass(const std::shared_ptr<AST>&);

        virtual ~Pass() = default;

        virtual bool run() = 0;
        virtual bool success();
        virtual void fail();

        bool error();
        bool error(const std::string&);
        bool error(const Node&, const std::string&);
        bool error(const Node*, const std::string&);
        bool error(const std::shared_ptr<Node>&, const std::string&);
        bool hasErrors() const;
        std::string errorPrintout();

    };

}

#endif
