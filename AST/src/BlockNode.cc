#include <BlockNode.h>

namespace avl {

    BlockNode::BlockNode(uint16_t t, const std::shared_ptr<BlockNode>& p):
        Statement(t),
        parent(p)
    {
    }

    CondBlockNode::CondBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_COND, p)
    {
    }

    IfBlockNode::IfBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_IF, p)
    {
    }

    LoopBlockNode::LoopBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_LOOP, p)
    {
    }

    FuncBlockNode::FuncBlockNode():
        BlockNode(BLOCK_FUNCTION)
    {
    }

    FuncBlockNode::FuncBlockNode(const std::shared_ptr<Identifier>& n, const std::shared_ptr<Node>& t):
        BlockNode(BLOCK_FUNCTION),
        name(n),
        type(t)
    {
    }

}
