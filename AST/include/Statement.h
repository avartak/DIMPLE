#ifndef STATEMENT_H
#define STATEMENT_H

#include <Node.h>
#include <Identifier.h>
#include <ExprNode.h>

namespace avl {

    enum Statement_ID {

        STATEMENT_CONTINUE,
        STATEMENT_BREAK,
        STATEMENT_RETURN,
        STATEMENT_CALL,
        STATEMENT_ASSIGN,
        STATEMENT_DEFINE,

        BLOCK_COND,
        BLOCK_IF,
        BLOCK_LOOP,
        BLOCK_FUNCTION

    };

    enum Storage_ID {

        STORAGE_REFERENCE,
        STORAGE_EXTERNAL,
        STORAGE_INTERNAL,
        STORAGE_STATIC,
        STORAGE_LOCAL,
        STORAGE_PRIVATE

    };

    struct Statement : public Node {

        uint16_t is;

        Statement(uint16_t);

        virtual ~Statement() = default;

    };

    struct ContinueStatement : public Statement {

        ContinueStatement();

    };

    struct BreakStatement : public Statement {

        BreakStatement();

    };

    struct ReturnStatement : public Statement {

        std::shared_ptr<Node> val;

        ReturnStatement(const std::shared_ptr<Node>&);

    };

    struct CallStatement : public Statement {

        std::shared_ptr<CallExprNode> exp;

        CallStatement(const std::shared_ptr<CallExprNode>&);

    };

    struct AssignStatement : public Statement {

        std::shared_ptr<AssignExprNode> exp;

        AssignStatement(const std::shared_ptr<AssignExprNode>&);

    };

    struct DefineStatement : public Statement {

        uint16_t storage;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        DefineStatement(uint16_t, const std::shared_ptr<Identifier>&, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);

    };

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

}

#endif
