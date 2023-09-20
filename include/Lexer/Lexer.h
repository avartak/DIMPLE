#ifndef LEXER_H
#define LEXER_H

#include <map>
#include <string>
#include <fstream>
#include <IO/Coordinate.h>

namespace dmp {

    enum Lexer_State_ID {

        LEXER_STATE_INIT,
        LEXER_STATE_INCLUDE,
        LEXER_STATE_WS,
        LEXER_STATE_ONELINE_COMMENT,
        LEXER_STATE_MULTILINE_COMMENT,
        LEXER_STATE_WORD,
        LEXER_STATE_NUM,
        LEXER_STATE_INT_BIN,
        LEXER_STATE_INT_OCT,
        LEXER_STATE_INT_HEX,
        LEXER_STATE_REAL_DOT,
        LEXER_STATE_REAL_EXP,
        LEXER_STATE_CHAR_START,
        LEXER_STATE_CHAR_NEXT,
        LEXER_STATE_CHAR_ESC,
        LEXER_STATE_CHAR_ESC_HEX,
        LEXER_STATE_CHAR_DONE,
        LEXER_STATE_STRING_START,
        LEXER_STATE_STRING_NEXT,
        LEXER_STATE_STRING_ESC,
        LEXER_STATE_STRING_ESC_HEX,
        LEXER_STATE_STRING_DONE,
	LEXER_STATE_TOTAL_NUM_STATES

    };

    struct Lexer {

        static const std::string bin;
        static const std::string oct;
        static const std::string dec;
        static const std::string hex;
        static const std::string letter;
        static const std::string charset;
        static std::map<int, std::string> symbols;
        static std::map<int, std::string> keywords;

        std::ifstream& file;
        bool input_error;
        int next_char;
        int state;
	Coordinate start;
	Coordinate end;
	Coordinate last;
	Coordinate next;
        std::string token_string;
        std::string token_buffer;

        int (*state_processors[LEXER_STATE_TOTAL_NUM_STATES])(bool exact, const std::string& token_buffer, int& state);

        Lexer(std::ifstream&);

        void populate_state_processors();
        void read();
        void append();
        void unappend();
        int  rule();
        int  lex();
        int  match(bool);

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
        RULE_LOOP,
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
        RULE_REM_ASSIGN,
        RULE_AND_ASSIGN,
        RULE_OR_ASSIGN,
        RULE_XOR_ASSIGN,
        RULE_BIT_RIGHT_ASSIGN,
        RULE_BIT_LEFT_ASSIGN,

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
