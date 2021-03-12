#include <IfBlockNode.h>

namespace avl {

    IfBlockNode::IfBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_IF, p)
    {
    }

}
