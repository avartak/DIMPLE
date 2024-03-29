#include <AST/ExprNode.h>
#include <AST/Token.h>
#include <Lexer/TokenID.h>

namespace dmp {

    std::map<int, uint16_t> ExprNode::unopsmap = {
        {TOKEN_DEREF     , UNARYOP_DEREFERENCE},
        {TOKEN_ADDRESS   , UNARYOP_ADDRESS    },
        {TOKEN_SIZE      , UNARYOP_SIZE       },
        {TOKEN_PLUS      , UNARYOP_PLUS       },
        {TOKEN_MINUS     , UNARYOP_NEGATE     },
        {TOKEN_NOT       , UNARYOP_NOT        },
        {TOKEN_COMPLEMENT, UNARYOP_COMPLEMENT }
    };

    std::map<int, uint16_t> ExprNode::binopsmap = {
        {TOKEN_LOG_OR    , BINARYOP_LOGICAL_OR   },
        {TOKEN_LOG_AND   , BINARYOP_LOGICAL_AND  },
        {TOKEN_EQUAL     , BINARYOP_EQUAL        },
        {TOKEN_NOT_EQUAL , BINARYOP_NOT_EQUAL    },
        {TOKEN_GEQ       , BINARYOP_GREATER_EQUAL},
        {TOKEN_LEQ       , BINARYOP_LESSER_EQUAL },
        {TOKEN_GREATER   , BINARYOP_GREATER      },
        {TOKEN_LESSER    , BINARYOP_LESSER       },
        {TOKEN_BIT_OR    , BINARYOP_BIT_OR       },
        {TOKEN_BIT_XOR   , BINARYOP_BIT_XOR      },
        {TOKEN_BIT_AND   , BINARYOP_BIT_AND      },
        {TOKEN_BIT_LEFT  , BINARYOP_SHIFT_LEFT   },
        {TOKEN_BIT_RIGHT , BINARYOP_SHIFT_RIGHT  },
        {TOKEN_PLUS      , BINARYOP_ADD          },
        {TOKEN_MINUS     , BINARYOP_SUBTRACT     },
        {TOKEN_MULTIPLY  , BINARYOP_MULTIPLY     },
        {TOKEN_DIVIDE    , BINARYOP_DIVIDE       },
        {TOKEN_REMAINDER , BINARYOP_REMAINDER    }
    };

    std::map<int, uint16_t> ExprNode::assopsmap = {
        {TOKEN_ASSIGN          , ASSIGNOP_ASSIGN          },
        {TOKEN_ADD_ASSIGN      , ASSIGNOP_ADD_ASSIGN      },
        {TOKEN_SUB_ASSIGN      , ASSIGNOP_SUB_ASSIGN      },
        {TOKEN_MUL_ASSIGN      , ASSIGNOP_MUL_ASSIGN      },
        {TOKEN_DIV_ASSIGN      , ASSIGNOP_DIV_ASSIGN      },
        {TOKEN_REM_ASSIGN      , ASSIGNOP_REM_ASSIGN      },
        {TOKEN_AND_ASSIGN      , ASSIGNOP_AND_ASSIGN      },
        {TOKEN_OR_ASSIGN       , ASSIGNOP_OR_ASSIGN       },
        {TOKEN_XOR_ASSIGN      , ASSIGNOP_XOR_ASSIGN      },
        {TOKEN_BIT_RIGHT_ASSIGN, ASSIGNOP_BIT_RIGHT_ASSIGN},
        {TOKEN_BIT_LEFT_ASSIGN , ASSIGNOP_BIT_LEFT_ASSIGN }
    };

    std::map<uint16_t, std::string> ExprNode::unopstrmap = {
        {UNARYOP_DEREFERENCE, "$"},
        {UNARYOP_ADDRESS    , "@"},
        {UNARYOP_SIZE       , "#"},
        {UNARYOP_PLUS       , "+"},
        {UNARYOP_NEGATE     , "-"},
        {UNARYOP_NOT        , "!"},
        {UNARYOP_COMPLEMENT , "~"}
    };

    std::map<uint16_t, std::string> ExprNode::binopstrmap = {
        {BINARYOP_LOGICAL_OR   , "||"},
        {BINARYOP_LOGICAL_AND  , "&&"},
        {BINARYOP_EQUAL        , "=="},
        {BINARYOP_NOT_EQUAL    , "!="},
        {BINARYOP_GREATER_EQUAL, ">="},
        {BINARYOP_LESSER_EQUAL , "<="},
        {BINARYOP_GREATER      , ">" },
        {BINARYOP_LESSER       , "<" },
        {BINARYOP_BIT_OR       , "|" },
        {BINARYOP_BIT_XOR      , "^" },
        {BINARYOP_BIT_AND      , "&" },
        {BINARYOP_SHIFT_LEFT   , "<<"},
        {BINARYOP_SHIFT_RIGHT  , ">>"},
        {BINARYOP_ADD          , "+" },
        {BINARYOP_SUBTRACT     , "-" },
        {BINARYOP_MULTIPLY     , "*" },
        {BINARYOP_DIVIDE       , "/" },
        {BINARYOP_REMAINDER    , "//"}
    };

    std::map<uint16_t, std::string> ExprNode::assopstrmap = {
        {ASSIGNOP_ASSIGN          , "="  },
        {ASSIGNOP_ADD_ASSIGN      , "+=" },
        {ASSIGNOP_SUB_ASSIGN      , "-=" },
        {ASSIGNOP_MUL_ASSIGN      , "*=" },
        {ASSIGNOP_DIV_ASSIGN      , "/=" },
        {ASSIGNOP_REM_ASSIGN      , "//="},
        {ASSIGNOP_AND_ASSIGN      , "&=" },
        {ASSIGNOP_OR_ASSIGN       , "|=" },
        {ASSIGNOP_XOR_ASSIGN      , "^=" },
        {ASSIGNOP_BIT_RIGHT_ASSIGN, ">>="},
        {ASSIGNOP_BIT_LEFT_ASSIGN , "<<="}
    };

    std::map<int, int> ExprNode::prec = {
        {TOKEN_LOG_OR    , 100},
        {TOKEN_LOG_AND   , 200},
        {TOKEN_EQUAL     , 300},
        {TOKEN_NOT_EQUAL , 300},
        {TOKEN_GEQ       , 400},
        {TOKEN_LEQ       , 400},
        {TOKEN_GREATER   , 400},
        {TOKEN_LESSER    , 400},
        {TOKEN_BIT_OR    , 500},
        {TOKEN_BIT_XOR   , 500},
        {TOKEN_BIT_AND   , 500},
        {TOKEN_BIT_LEFT  , 600},
        {TOKEN_BIT_RIGHT , 600},
        {TOKEN_PLUS      , 700},
        {TOKEN_MINUS     , 700},
        {TOKEN_MULTIPLY  , 800},
        {TOKEN_DIVIDE    , 800},
        {TOKEN_REMAINDER , 800}
    };

    ExprNode::ExprNode(uint16_t t):
        Node(NODE_EXPRNODE),
        is(t)
    {
    }

    AssignExprNode::AssignExprNode(uint16_t o, const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r):
        ExprNode(EXPR_ASSIGN),
        op(o),
        lhs(l),
        rhs(r)
    {
    }

    BinaryExprNode::BinaryExprNode(uint16_t o, const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r):
        ExprNode(EXPR_BINARY),
        op(o),
        lhs(l),
        rhs(r)
    {
    }

    CallExprNode::CallExprNode(const std::shared_ptr<Node>& f, const std::vector<std::shared_ptr<Node> >& a):
        ExprNode(EXPR_CALL),
        func(f),
        args(a)
    {
    }

    UnaryExprNode::UnaryExprNode(uint16_t o, const std::shared_ptr<Node>& e):
        ExprNode(EXPR_UNARY),
        op(o),
        exp(e)
    {
    }

}
