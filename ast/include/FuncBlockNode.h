#ifndef FUNCTIONBLOCKNODE_H
#define FUNCTIONBLOCKNODE_H

#include <BlockNode.h>
#include <Identifier.h>

namespace avl {

    struct FuncBlockNode : public BlockNode {
        
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;

        FuncBlockNode();
        FuncBlockNode(const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&);

    };

}

#endif
