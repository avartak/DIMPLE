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
        std::map<std::string, std::shared_ptr<Identifier> > symbols;

        BlockNode(uint16_t, const std::shared_ptr<BlockNode>& = nullptr);

    };

    struct CondBlockNode : public BlockNode {

        std::shared_ptr<Node> condition;

        CondBlockNode(const std::shared_ptr<BlockNode>&);

    };

    struct IfBlockNode : public BlockNode {

        IfBlockNode(const std::shared_ptr<BlockNode>&);

    };

    struct LoopBlockNode : public BlockNode {

        LoopBlockNode(const std::shared_ptr<BlockNode>&);

    };

    struct FuncBlockNode : public BlockNode {

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;

        FuncBlockNode();
        FuncBlockNode(const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&);

    };

}

#endif
