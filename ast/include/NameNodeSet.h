#ifndef NAMENODESET_H
#define NAMENODESET_H

#include <vector>
#include <NameNode.h>

namespace avl {

    struct NameNodeSet : public Node {

        std::vector<NameNode> set;
        
        NameNodeSet();

    };

}

#endif
