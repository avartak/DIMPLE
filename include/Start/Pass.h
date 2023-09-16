#ifndef PASS_H
#define PASS_H

#include <memory>
#include <Start/InputManager.h>
#include <Start/Error.h>
#include <AST/AST.h>
#include <AST/Node.h>
#include <IR/GST.h>

namespace dmp {

    template<typename T>
    struct Pass {

        InputManager* const input;
        AST* const ast;
        GST* const gst;
        std::shared_ptr<T> result;
        std::vector<Error> errors;

        Pass(InputManager*, AST*, GST*);

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
