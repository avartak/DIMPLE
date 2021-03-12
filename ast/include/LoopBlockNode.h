#ifndef LOOPBLOCKNODE_H
#define LOOPBLOCKNODE_H

#include <BlockNode.h>

namespace avl {

    struct LoopBlockNode : public BlockNode {
        
        LoopBlockNode(const std::shared_ptr<BlockNode>&);

    };

}

#endif
