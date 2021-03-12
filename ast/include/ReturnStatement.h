#ifndef RETURNSTATEMENT_H
#define RETURNSTATEMENT_H

#include <memory>
#include <Statement.h>

namespace avl {

    struct ReturnStatement : public Statement {

        std::shared_ptr<Node> val;

        ReturnStatement(const std::shared_ptr<Node>&);

    };

}

#endif
