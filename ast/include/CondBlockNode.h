#ifndef CONDBLOCKNODE_H
#define CONDBLOCKNODE_H

#include <BlockNode.h>

namespace avl {

    struct CondBlockNode : public BlockNode {
        
        std::shared_ptr<Node> condition;

        CondBlockNode(const std::shared_ptr<BlockNode>&);
        CondBlockNode(const std::shared_ptr<BlockNode>&, const std::shared_ptr<Node>&);

    };

}

#endif
