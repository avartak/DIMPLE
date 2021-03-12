#ifndef CALLSTATEMENT_H
#define CALLSTATEMENT_H

#include <memory>
#include <Statement.h>
#include <CallExprNode.h>

namespace avl {

    struct CallStatement : public Statement {

        std::shared_ptr<CallExprNode> exp;

        CallStatement(const std::shared_ptr<CallExprNode>&);

    };

}

#endif
