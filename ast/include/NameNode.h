#ifndef NAMENODE_H
#define NAMENODE_H

#include <memory>
#include <vector>
#include <Node.h>
#include <Identifier.h>

namespace avl {

    struct NameNode : public Node {

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> node;

        NameNode();
        NameNode(const std::shared_ptr<Node>&);
        NameNode(const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&);

    };

    struct NameNodeSet : public Node {

        std::vector<NameNode> set;

        NameNodeSet();

    };

}

#endif
