#ifndef BOOLNODE_H
#define BOOLNODE_H

#include <memory>
#include <ExprNode.h>
#include <Token.h>

namespace avl {

    struct BoolNode : public ExprNode {

        bool literal;

        BoolNode(const std::shared_ptr<Token>&, bool);

        static std::shared_ptr<BoolNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
