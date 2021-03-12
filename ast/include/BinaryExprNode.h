#ifndef BINARYEXPRNODE_H
#define BINARYEXPRNODE_H

#include <memory>
#include <ExprNode.h>

namespace avl {

    struct BinaryExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> lhs;
        std::shared_ptr<Node> rhs;

        BinaryExprNode(uint16_t, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);

    };

}

#endif
