#include <CondBlockNode.h>

namespace avl {

    CondBlockNode::CondBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_COND, p)
    {
    }

    CondBlockNode::CondBlockNode(const std::shared_ptr<BlockNode>& p, const std::shared_ptr<Node>& c):
        BlockNode(BLOCK_COND, p),
        condition(c)
    {
    }

}
