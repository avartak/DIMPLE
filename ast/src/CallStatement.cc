#include <CallStatement.h>

namespace avl {

    CallStatement::CallStatement(const std::shared_ptr<CallExprNode>& ce):
        Statement(STATEMENT_CALL),
        exp(ce)
    {
        loc = ce->loc;
    }

}
