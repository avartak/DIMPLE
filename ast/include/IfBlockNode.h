#ifndef IFBLOCKNODE_H
#define IFBLOCKNODE_H

#include <BlockNode.h>

namespace avl {

    struct IfBlockNode : public BlockNode {
        
        IfBlockNode(const std::shared_ptr<BlockNode>&);

    };

}

#endif
