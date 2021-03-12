#include <AssignStatement.h>

namespace avl {

    AssignStatement::AssignStatement(const std::shared_ptr<AssignExprNode>& ae):
        Statement(STATEMENT_ASSIGN),
        exp(ae)
    {
        loc = ae->loc;
    }

}
