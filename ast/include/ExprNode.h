#ifndef EXPRNODE_H
#define EXPRNODE_H

#include <map>
#include <Node.h>

namespace avl {

    enum Expr_ID {

        EXPR_UNARY,
        EXPR_BINARY,
        EXPR_CALL,
        EXPR_ASSIGN,
        EXPR_ARG_INDEX,
        EXPR_ONE,
        EXPR_BOOL,
        EXPR_INT,
        EXPR_REAL,
        EXPR_CHAR,
        EXPR_STRING

    };

    enum Ops_ID {

        UNARYOP_NOP,
        UNARYOP_DEREFERENCE,
        UNARYOP_ADDRESS,
        UNARYOP_SIZE,
        UNARYOP_PLUS,
        UNARYOP_NEGATE,
        UNARYOP_NOT,
        UNARYOP_COMPLEMENT,

        BINARYOP_STR_CONCAT,
        BINARYOP_RECAST,
        BINARYOP_MEMBER,
        BINARYOP_ELEMENT,
        BINARYOP_ADD,
        BINARYOP_SUBTRACT,
        BINARYOP_MULTIPLY,
        BINARYOP_DIVIDE,
        BINARYOP_MOD,
        BINARYOP_SHIFT_LEFT,
        BINARYOP_SHIFT_RIGHT,
        BINARYOP_BIT_OR,
        BINARYOP_BIT_AND,
        BINARYOP_BIT_XOR,
        BINARYOP_LOGICAL_OR,
        BINARYOP_LOGICAL_AND,
        BINARYOP_EQUAL,
        BINARYOP_NOT_EQUAL,
        BINARYOP_GREATER,
        BINARYOP_LESSER,
        BINARYOP_GREATER_EQUAL,
        BINARYOP_LESSER_EQUAL,

        ASSIGNOP_ASSIGN,
        ASSIGNOP_ADD_ASSIGN,
        ASSIGNOP_SUB_ASSIGN,
        ASSIGNOP_MUL_ASSIGN,
        ASSIGNOP_DIV_ASSIGN,
        ASSIGNOP_REM_ASSIGN,
        ASSIGNOP_AND_ASSIGN,
        ASSIGNOP_OR_ASSIGN,
        ASSIGNOP_XOR_ASSIGN,
        ASSIGNOP_BIT_RIGHT_ASSIGN,
        ASSIGNOP_BIT_LEFT_ASSIGN
    };

    struct ExprNode : public Node {

        static std::map<int, int> prec;
        static std::map<int, uint16_t> unopsmap;
        static std::map<int, uint16_t> binopsmap;
        static std::map<int, uint16_t> assopsmap;

        uint16_t is;

        ExprNode(uint16_t);

        virtual ~ExprNode() = default;

        static inline uint16_t unopFromToken(int t) {
            return unopsmap[t];
        }

        static inline uint16_t binopFromToken(int t) {
            return binopsmap[t];
        }

        static inline uint16_t assopFromToken(int t) {
            return assopsmap[t];
        }

        static inline int precedence(int t) {
            return prec[t];
        }

    };

}

#endif
