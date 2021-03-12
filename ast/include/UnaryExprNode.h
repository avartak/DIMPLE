#ifndef UNARYEXPRNODE_H
#define UNARYEXPRNODE_H

#include <memory>
#include <ExprNode.h>

namespace avl {

    struct UnaryExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> exp;

        UnaryExprNode(uint16_t, const std::shared_ptr<Node>&);

    };

}

#endif
