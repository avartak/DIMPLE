#ifndef STRINGNODE_H
#define STRINGNODE_H

#include <string>
#include <memory>
#include <ExprNode.h>
#include <Token.h>

namespace avl {

    struct StringNode : public ExprNode {

        std::string literal;
        std::string str;

        StringNode(const std::shared_ptr<Token>&, const std::string&);

        static std::shared_ptr<StringNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
