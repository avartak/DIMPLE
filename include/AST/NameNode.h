#ifndef NAMENODE_H
#define NAMENODE_H

#include <memory>
#include <vector>
#include <AST/Node.h>
#include <AST/Identifier.h>

namespace dmp {

    enum Argument_Attribute_ID {

        PASS_BY_VALUE = 0,
        PASS_BY_REFERENCE = 1

    };

    struct NameNode : public Node {

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> node;
        uint64_t attr;

        NameNode();
        NameNode(const std::shared_ptr<Node>&);
        NameNode(const std::shared_ptr<Node>&, uint64_t);
        NameNode(const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&);
        NameNode(const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&, uint64_t);

    };

    struct NameNodeSet : public Node {

        std::vector<NameNode> set;

        NameNodeSet();

    };

}

#endif
