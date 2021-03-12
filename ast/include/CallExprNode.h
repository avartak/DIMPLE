#ifndef CALLEXPRNODE_H
#define CALLEXPRNODE_H

#include <memory>
#include <vector>
#include <ExprNode.h>

namespace avl {

    struct CallExprNode : public ExprNode {

        std::shared_ptr<Node> func;
        std::vector<std::shared_ptr<Node> > args;

        CallExprNode(const std::shared_ptr<Node>&, const std::vector<std::shared_ptr<Node> >&);
    };

}

#endif
