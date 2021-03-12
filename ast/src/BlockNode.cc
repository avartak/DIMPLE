#include <BlockNode.h>

namespace avl {

    BlockNode::BlockNode(uint16_t t, const std::shared_ptr<BlockNode>& p):
        Statement(t),
        parent(p)
    {
    }

    bool BlockNode::isInLoop() const {
        auto p = parent;
        while (p) {
            if (p->is == BLOCK_LOOP) {
                return true;
            }
            p = p->parent;
        }        
        return false;
    }    
}
