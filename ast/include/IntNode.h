#ifndef INTNODE_H
#define INTNODE_H

#include <memory>
#include <string>
#include <ExprNode.h>
#include <Token.h>

namespace avl {

    struct IntNode : public ExprNode {

        uint64_t literal;
        std::string str;

        IntNode(const std::shared_ptr<Token>&, uint64_t);

        static std::shared_ptr<IntNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
