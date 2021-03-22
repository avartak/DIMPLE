#ifndef STATEMENT_H
#define STATEMENT_H

#include <Node.h>

namespace avl {

    enum Statement_ID {

        STATEMENT_CONTINUE,
        STATEMENT_BREAK,
        STATEMENT_RETURN,
        STATEMENT_CALL,
        STATEMENT_ASSIGN,
        STATEMENT_DEFINITION,

        BLOCK_COND,
        BLOCK_IF,
        BLOCK_LOOP,
        BLOCK_FUNCTION

    };

    struct Statement : public Node {

        uint16_t is;

        Statement(uint16_t);

        virtual ~Statement() = default;

    };

}

#endif
