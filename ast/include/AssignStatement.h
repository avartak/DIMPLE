#ifndef ASSIGNSTATEMENT_H
#define ASSIGNSTATEMENT_H

#include <memory>
#include <Statement.h>
#include <AssignExprNode.h>

namespace avl {

    struct AssignStatement : public Statement {

        std::shared_ptr<AssignExprNode> exp;

        AssignStatement(const std::shared_ptr<AssignExprNode>&);

    };

}

#endif
