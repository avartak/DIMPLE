#include <ReturnStatement.h>

namespace avl {

    ReturnStatement::ReturnStatement(const std::shared_ptr<Node>& v):
        Statement(STATEMENT_RETURN),
        val(v)
    {
    }

}
