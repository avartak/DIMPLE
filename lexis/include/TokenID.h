#ifndef TOKENID_H
#define TOKENID_H

namespace avl {

    enum TokenID {

        TOKEN_NONE = 256,
        TOKEN_EOF,
        TOKEN_UNDEF,
        TOKEN_ERROR,

        TOKEN_INCLUDE,
        TOKEN_FILENAME,
        
        TOKEN_INT,
        TOKEN_REAL,
        TOKEN_TRUE,
        TOKEN_FALSE,
        TOKEN_CHAR,
        TOKEN_STRING,
        
        TOKEN_IDENT,
        TOKEN_START,
        
        TOKEN_BOOL,
        TOKEN_UINT8,
        TOKEN_UINT16,
        TOKEN_UINT32,
        TOKEN_UINT64,
        TOKEN_INT8,
        TOKEN_INT16,
        TOKEN_INT32,
        TOKEN_INT64,
        TOKEN_REAL32,
        TOKEN_REAL64,
        TOKEN_GENERIC_POINTER,
        TOKEN_STRUCT,
        TOKEN_UNION,
        TOKEN_FUNCTION,
        TOKEN_EXTERN,
        TOKEN_PACKED,
        
        TOKEN_RETURN,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_LOOP,
        TOKEN_BREAK,
        TOKEN_CONTINUE,
       
        TOKEN_CURLY_OPEN,
        TOKEN_CURLY_CLOSE,
        TOKEN_SQUARE_OPEN,
        TOKEN_SQUARE_CLOSE,
        TOKEN_ROUND_OPEN,
        TOKEN_ROUND_CLOSE,
        TOKEN_COMMA,
        TOKEN_SEMICOLON,
 
        TOKEN_DECLARE,
        TOKEN_REPRESENTS,
        TOKEN_DEFINE,
        TOKEN_RETURNS,
        TOKEN_CAST,

        TOKEN_POINTER,
        TOKEN_ADDRESS,
        TOKEN_SIZE,
        TOKEN_DEREF,
        TOKEN_DOT,

        TOKEN_PLUS,
        TOKEN_MINUS,
        TOKEN_MULTIPLY,
        TOKEN_DIVIDE,
        TOKEN_REMAINDER,

        TOKEN_COMPLEMENT,
        TOKEN_BIT_AND,
        TOKEN_BIT_OR,
        TOKEN_BIT_XOR,
        TOKEN_BIT_RIGHT,
        TOKEN_BIT_LEFT,

        TOKEN_GREATER,
        TOKEN_LESSER,
        TOKEN_GEQ,
        TOKEN_LEQ,
        TOKEN_EQUAL,
        TOKEN_NOT_EQUAL,
        TOKEN_NOT,
        TOKEN_LOG_AND,
        TOKEN_LOG_OR,

        TOKEN_ASSIGN,
        TOKEN_ADD_ASSIGN,
        TOKEN_SUB_ASSIGN,
        TOKEN_MUL_ASSIGN,
        TOKEN_DIV_ASSIGN,
        TOKEN_REM_ASSIGN,
        TOKEN_AND_ASSIGN,
        TOKEN_OR_ASSIGN,
        TOKEN_XOR_ASSIGN,
        TOKEN_BIT_RIGHT_ASSIGN,
        TOKEN_BIT_LEFT_ASSIGN

    };

}

#endif
