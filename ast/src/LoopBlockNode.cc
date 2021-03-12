#include <LoopBlockNode.h>

namespace avl {

    LoopBlockNode::LoopBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_LOOP, p)
    {
    }

}
