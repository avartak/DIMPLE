#include <Statement.h>

namespace dmp {

    Statement::Statement(uint16_t t):
        Node(NODE_STATEMENT),
        is(t)
    {
    }

    ContinueStatement::ContinueStatement():
        Statement(STATEMENT_CONTINUE)
    {
    }

    BreakStatement::BreakStatement():
        Statement(STATEMENT_BREAK)
    {
    }

    ReturnStatement::ReturnStatement(const std::shared_ptr<Node>& v):
        Statement(STATEMENT_RETURN),
        val(v)
    {
    }

    CallStatement::CallStatement(const std::shared_ptr<CallExprNode>& ce):
        Statement(STATEMENT_CALL),
        exp(ce)
    {
        loc = ce->loc;
    }

    AssignStatement::AssignStatement(const std::shared_ptr<AssignExprNode>& ae):
        Statement(STATEMENT_ASSIGN),
        exp(ae)
    {
        loc = ae->loc;
    }

    DefineStatement::DefineStatement(uint16_t s, const std::shared_ptr<Identifier>& n, const std::shared_ptr<Node>& t, const std::shared_ptr<Node>& d):
        Statement(STATEMENT_DEFINE),
        storage(s),
        name(n),
        type(t),
        def(d)
    {
        loc = name->loc;
        loc.end = def->loc.end;
    }

    BlockNode::BlockNode(uint16_t t, const std::shared_ptr<BlockNode>& p):
        Statement(t),
        parent(p)
    {
    }

    CondBlockNode::CondBlockNode(const std::shared_ptr<BlockNode>& p):
        BlockNode(BLOCK_COND, p)
    {
    }

}
