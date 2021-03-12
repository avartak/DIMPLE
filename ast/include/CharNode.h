#ifndef CHARNODE_H
#define CHARNODE_H

#include <memory>
#include <ExprNode.h>
#include <Token.h>

namespace avl {

    struct CharNode : public ExprNode {

        char literal;
        std::string str;

        CharNode(const std::shared_ptr<Token>&, char);

        static std::shared_ptr<CharNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
