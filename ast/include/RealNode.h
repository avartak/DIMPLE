#ifndef REALNODE_H
#define REALNODE_H

#include <memory>
#include <ExprNode.h>
#include <Token.h>

namespace avl {

    struct RealNode : public ExprNode {

        double literal;
        std::string str;

        RealNode(const std::shared_ptr<Token>&, double);

        static std::shared_ptr<RealNode> construct(const std::shared_ptr<Token>&);

    };

}

#endif
