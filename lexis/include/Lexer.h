/*

The Lexer performs lexical analysis for our compiler. 
It reads text from an input file character-by-character,
and constructs tokens that are then passed on to the Parser.
The Lexer stores all the pertinent information about a token
- The token type or ID (int) that is returned by the scan() function
- The token string (str : std::string) that contains the textual representation of the token
- The line on which the first character of the token appears (start_line : int)
- The column on which the first character of the token appears (start_column : int)
- The line on which the last character of the token appears (end_line : int)
- the column on which the last character of the token appears (end_column : int)

Line and column numbers start from 1

*/

#ifndef LEXER_H
#define LEXER_H

#include <map>
#include <string>
#include <memory>
#include <fstream>

namespace avl {

    enum Lexer_State_ID {

        LEXER_STATE_INIT,
        LEXER_STATE_INCL,
        LEXER_STATE_WS,
        LEXER_STATE_ONELINE_COMMENT,
        LEXER_STATE_MULTILINE_COMMENT,
        LEXER_STATE_WORD,
        LEXER_STATE_NUM,
        LEXER_STATE_INT,
        LEXER_STATE_REAL,
        LEXER_STATE_CHAR,
        LEXER_STATE_STRING

    };

    struct Lexer {

        static std::string bin;
        static std::string oct;
        static std::string dec;
        static std::string hex;
        static std::string letter;
        static std::map<int, std::string> symbols;
        static std::map<int, std::string> keywords;

        std::ifstream& file;
        bool input_error;
        int next;
        int state;
        uint32_t start_line;
        uint16_t start_column;
        uint32_t end_line;
        uint16_t end_column;
        uint32_t last_line;
        uint16_t last_column;
        uint32_t next_line;
        uint16_t next_column;
        std::string str;
        std::string token;

        Lexer(std::ifstream&);

        void read();
        void append();
        int rule();
        int scan();

        std::shared_ptr<int> match(bool);
        bool isWS(bool);
        bool isOneLineComment(bool);
        bool isMultiLineComment(bool);
        bool isIdentifier(bool);
        bool isInt(bool);
        bool isReal(bool);
        bool isChar(bool);
        bool isString(bool);
        bool nextChar(std::size_t&, bool);

    };

    enum Rule_ID {

        RULE_ERROR,
        RULE_UNDEF,
        RULE_EOF,

        RULE_INCLUDE,
        RULE_FILENAME,

        RULE_WS,
        RULE_ONELINE_COMMENT,
        RULE_MULTILINE_COMMENT,

        RULE_INT,
        RULE_TRUE,
        RULE_FALSE,
        RULE_CHAR,
        RULE_STRING,
        RULE_REAL,

        RULE_IDENT,
        RULE_MAIN,

        RULE_BOOL,
        RULE_UINT8,
        RULE_UINT16,
        RULE_UINT32,
        RULE_UINT64,
        RULE_INT8,
        RULE_INT16,
        RULE_INT32,
        RULE_INT64,
        RULE_REAL32,
        RULE_REAL64,
        RULE_GENERIC_POINTER,
        RULE_STRUCT,
        RULE_UNION,
        RULE_FUNCTION,
        RULE_PACKED,
        RULE_EXTERN,

        RULE_IF,
        RULE_ELSE,
        RULE_FOR,
        RULE_BREAK,
        RULE_CONTINUE,
        RULE_RETURN,

        RULE_DECLARE,
        RULE_REPRESENTS,
        RULE_DEFINE,
        RULE_RETURNS,
        RULE_CAST,

        RULE_NOT,
        RULE_LOG_AND,
        RULE_LOG_OR,
        RULE_EQUAL,
        RULE_NOT_EQUAL,
        RULE_GREATER,
        RULE_LESSER,
        RULE_GEQ,
        RULE_LEQ,

        RULE_COMPLEMENT,      
        RULE_BIT_AND,   
        RULE_BIT_OR,
        RULE_BIT_XOR,         
        RULE_BIT_RIGHT,
        RULE_BIT_LEFT,

        RULE_PLUS,        
        RULE_MINUS,       
        RULE_MULTIPLY,
        RULE_DIVIDE,      
        RULE_REMAINDER,

        RULE_POINTER,
        RULE_ADDRESS,          
        RULE_SIZE,        
        RULE_DEREF,      
        RULE_DOT,         

        RULE_ASSIGN,
        RULE_ADD_ASSIGN,
        RULE_SUB_ASSIGN,
        RULE_MUL_ASSIGN,
        RULE_DIV_ASSIGN,
        RULE_MOD_ASSIGN,
        RULE_AND_ASSIGN,
        RULE_OR_ASSIGN,
        RULE_XOR_ASSIGN,
        RULE_BIT_RIGHT_ASSIGN,
        RULE_BIT_LEFT_ASSIGN,
        RULE_INC,
        RULE_DEC,

        RULE_CURLY_OPEN,  
        RULE_CURLY_CLOSE,
        RULE_SQUARE_OPEN, 
        RULE_SQUARE_CLOSE,
        RULE_ROUND_OPEN,  
        RULE_ROUND_CLOSE,
        RULE_COMMA,       
        RULE_SEMICOLON

    };

}

#endif
