#ifndef ASSIGNEXPRNODE_H
#define ASSIGNEXPRNODE_H

#include <memory>
#include <ExprNode.h>

namespace avl {

    struct AssignExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> lhs;
        std::shared_ptr<Node> rhs;

        AssignExprNode(uint16_t, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
    };

}

#endif
