#ifndef EXPRNODE_H
#define EXPRNODE_H

#include <map>
#include <vector>
#include <memory>
#include <Node.h>

namespace dmp {

    enum Expr_ID {

        EXPR_UNARY,
        EXPR_BINARY,
        EXPR_CALL,
        EXPR_ASSIGN,
        EXPR_BOOL,
        EXPR_INT,
        EXPR_REAL,
        EXPR_CHAR,
        EXPR_STRING

    };

    enum Ops_ID {

        UNARYOP_DEREFERENCE,
        UNARYOP_ADDRESS,
        UNARYOP_SIZE,
        UNARYOP_PLUS,
        UNARYOP_NEGATE,
        UNARYOP_NOT,
        UNARYOP_COMPLEMENT,

        BINARYOP_RECAST,
        BINARYOP_MEMBER,
        BINARYOP_ELEMENT,
        BINARYOP_ADD,
        BINARYOP_SUBTRACT,
        BINARYOP_MULTIPLY,
        BINARYOP_DIVIDE,
        BINARYOP_REMAINDER,
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
        static std::map<uint16_t, std::string> unopstrmap;
        static std::map<uint16_t, std::string> binopstrmap;
        static std::map<uint16_t, std::string> assopstrmap;

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

        static inline std::string unopstring(uint16_t o) {
            return unopstrmap[o];
        }

        static inline std::string binopstring(uint16_t o) {
            return binopstrmap[o];
        }

        static inline std::string assopstring(uint16_t o) {
            return assopstrmap[o];
        }

        static inline int precedence(int t) {
            return prec[t];
        }

        inline bool isLiteralNode() {
            return (is == EXPR_INT || is == EXPR_REAL || is == EXPR_BOOL || is == EXPR_CHAR || is == EXPR_STRING);
        }
    };

    struct AssignExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> lhs;
        std::shared_ptr<Node> rhs;

        AssignExprNode(uint16_t, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
    };

    struct BinaryExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> lhs;
        std::shared_ptr<Node> rhs;

        BinaryExprNode(uint16_t, const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);

    };

    struct CallExprNode : public ExprNode {

        std::shared_ptr<Node> func;
        std::vector<std::shared_ptr<Node> > args;

        CallExprNode(const std::shared_ptr<Node>&, const std::vector<std::shared_ptr<Node> >&);
    };

    struct UnaryExprNode : public ExprNode {

        uint16_t op;
        std::shared_ptr<Node> exp;

        UnaryExprNode(uint16_t, const std::shared_ptr<Node>&);

    };

}

#endif
