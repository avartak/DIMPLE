#include <FuncBlockNode.h>

namespace avl {

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
