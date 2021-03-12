#ifndef BLOCKNODE_H
#define BLOCKNODE_H

#include <memory>
#include <map>
#include <vector>
#include <Statement.h>
#include <Identifier.h>

namespace avl {

    struct BlockNode : public Statement {

        std::shared_ptr<BlockNode> parent;
        std::vector<std::shared_ptr<Statement> > body;
        std::map<std::string, std::shared_ptr<Identifier> > vars;

        BlockNode(uint16_t, const std::shared_ptr<BlockNode>& = nullptr);

        bool isInLoop() const;
    };

}

#endif
