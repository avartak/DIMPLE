#include <Lexer/Lexer.h>
#include <Lexer/TokenID.h>

namespace dmp {

    const std::string Lexer::bin     = "01";
    const std::string Lexer::oct     = "01234567";
    const std::string Lexer::dec     = "0123456789";
    const std::string Lexer::hex     = "0123456789abcdefABCDEF";
    const std::string Lexer::wspace  = " \t\n\r\v\f";
    const std::string Lexer::letter  = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string Lexer::escchar = "\a\b\t\n\r\v\f\'\"\\";

    std::map<int, std::string> Lexer::symbols = {
        { RULE_CURLY_OPEN,       "{"        },
        { RULE_CURLY_CLOSE,      "}"        },
        { RULE_SQUARE_OPEN,      "["        },
        { RULE_SQUARE_CLOSE,     "]"        },
        { RULE_ROUND_OPEN,       "("        },
        { RULE_ROUND_CLOSE,      ")"        },
        { RULE_COMMA,            ","        },
        { RULE_SEMICOLON,        ";"        },

        { RULE_DECLARE,          ":"        },
        { RULE_REPRESENTS,       "::"       },
        { RULE_DEFINE,           ":="       },
        { RULE_RETURNS,          "->"       },
        { RULE_CAST,             "=>"       },

        { RULE_PLUS,             "+"        },
        { RULE_MINUS,            "-"        },
        { RULE_MULTIPLY,         "*"        },
        { RULE_DIVIDE,           "/"        },
        { RULE_REMAINDER,        "//"       },

        { RULE_POINTER,          "%"        },
        { RULE_ADDRESS,          "@"        },
        { RULE_SIZE,             "#"        },
        { RULE_DEREF,            "$"        },
        { RULE_DOT,              "."        },

        { RULE_COMPLEMENT,       "~"        },
        { RULE_BIT_AND,          "&"        },
        { RULE_BIT_OR,           "|"        },
        { RULE_BIT_XOR,          "^"        },
        { RULE_BIT_RIGHT,        ">>"       },
        { RULE_BIT_LEFT,         "<<"       },

        { RULE_GREATER,          ">"        },
        { RULE_LESSER,           "<"        },
        { RULE_GEQ,              ">="       },
        { RULE_LEQ,              "<="       },
        { RULE_EQUAL,            "=="       },
        { RULE_NOT_EQUAL,        "!="       },
        { RULE_NOT,              "!"        },
        { RULE_LOG_AND,          "&&"       },
        { RULE_LOG_OR,           "!!"       },

        { RULE_ASSIGN,           "="        },
        { RULE_ADD_ASSIGN,       "+="       },
        { RULE_SUB_ASSIGN,       "-="       },
        { RULE_MUL_ASSIGN,       "*="       },
        { RULE_DIV_ASSIGN,       "/="       },
        { RULE_REM_ASSIGN,       "//="      },
        { RULE_AND_ASSIGN,       "&="       },
        { RULE_OR_ASSIGN,        "|="       },
        { RULE_XOR_ASSIGN,       "^="       },
        { RULE_BIT_RIGHT_ASSIGN, ">>="      },
        { RULE_BIT_LEFT_ASSIGN,  "<<="      },

        { RULE_GENERIC_POINTER,  "%?"       }
    };

    std::map<int, std::string> Lexer::keywords = {
        { RULE_INCLUDE,          "include"  },

        { RULE_MAIN,             "main"     },

        { RULE_TRUE,             "true"     },
        { RULE_FALSE,            "false"    },

        { RULE_BOOL,             "bool"     },
        { RULE_UINT8,            "uint8"    },
        { RULE_UINT16,           "uint16"   },
        { RULE_UINT32,           "uint32"   },
        { RULE_UINT64,           "uint64"   },
        { RULE_INT8,             "int8"     },
        { RULE_INT16,            "int16"    },
        { RULE_INT32,            "int32"    },
        { RULE_INT64,            "int64"    },
        { RULE_REAL32,           "real32"   },
        { RULE_REAL64,           "real64"   },
        { RULE_STRUCT,           "struct"   },
        { RULE_UNION,            "union"    },
        { RULE_FUNCTION,         "func"     },
        { RULE_EXTERN,           "extern"   },
        { RULE_PACKED,           "packed"   },

        { RULE_IF,               "if"       },
        { RULE_ELSE,             "else"     },
        { RULE_LOOP,             "loop"     },
        { RULE_BREAK,            "break"    },
        { RULE_CONTINUE,         "continue" },
        { RULE_RETURN,           "return"   }
    };

    Lexer::Lexer(std::ifstream& f):
        file(f),
        input_error(false),
        next_char(0),
        state(LEXER_STATE_INIT),
	start(1, 1),
	end(1, 1),
	last(1, 1),
	next(1, 1),
        token_string(""),
        token_buffer("")
    {
        populate_state_processors();
        read();
    }

    int Lexer::lex() {

        while (true) {

            auto rl = rule();
            
            switch (rl) {
                case RULE_EOF              : return TOKEN_EOF;
                case RULE_ERROR            : return TOKEN_ERROR;
                case RULE_UNDEF            : return TOKEN_UNDEF;
            
                case RULE_INCLUDE          : state = LEXER_STATE_INCLUDE; return TOKEN_INCLUDE;
                case RULE_FILENAME         : return TOKEN_FILENAME;
 
                case RULE_WS               : break;
                case RULE_ONELINE_COMMENT  : break;
                case RULE_MULTILINE_COMMENT: break;
            
                case RULE_INT              : return TOKEN_INT;
                case RULE_REAL             : return TOKEN_REAL;
                case RULE_TRUE             : return TOKEN_TRUE;
                case RULE_FALSE            : return TOKEN_FALSE;
                case RULE_CHAR             : return TOKEN_CHAR;
                case RULE_STRING           : return TOKEN_STRING;

                case RULE_IDENT            : return TOKEN_IDENT;
                case RULE_MAIN             : return TOKEN_MAIN;

                case RULE_BOOL             : return TOKEN_BOOL;
                case RULE_UINT8            : return TOKEN_UINT8;
                case RULE_UINT16           : return TOKEN_UINT16;
                case RULE_UINT32           : return TOKEN_UINT32;
                case RULE_UINT64           : return TOKEN_UINT64;
                case RULE_INT8             : return TOKEN_INT8;
                case RULE_INT16            : return TOKEN_INT16;
                case RULE_INT32            : return TOKEN_INT32;
                case RULE_INT64            : return TOKEN_INT64;
                case RULE_REAL32           : return TOKEN_REAL32;
                case RULE_REAL64           : return TOKEN_REAL64;
                case RULE_GENERIC_POINTER  : return TOKEN_GENERIC_POINTER;
                case RULE_STRUCT           : return TOKEN_STRUCT;
                case RULE_UNION            : return TOKEN_UNION;
                case RULE_FUNCTION         : return TOKEN_FUNCTION;
                case RULE_EXTERN           : return TOKEN_EXTERN;
                case RULE_PACKED           : return TOKEN_PACKED;

                case RULE_IF               : return TOKEN_IF;
                case RULE_ELSE             : return TOKEN_ELSE;
                case RULE_LOOP             : return TOKEN_LOOP;
                case RULE_BREAK            : return TOKEN_BREAK;
                case RULE_CONTINUE         : return TOKEN_CONTINUE;
                case RULE_RETURN           : return TOKEN_RETURN;

                case RULE_CURLY_OPEN       : return TOKEN_CURLY_OPEN;
                case RULE_CURLY_CLOSE      : return TOKEN_CURLY_CLOSE;
                case RULE_SQUARE_OPEN      : return TOKEN_SQUARE_OPEN;
                case RULE_SQUARE_CLOSE     : return TOKEN_SQUARE_CLOSE;
                case RULE_ROUND_OPEN       : return TOKEN_ROUND_OPEN;
                case RULE_ROUND_CLOSE      : return TOKEN_ROUND_CLOSE;
                case RULE_COMMA            : return TOKEN_COMMA;
                case RULE_SEMICOLON        : return TOKEN_SEMICOLON;

                case RULE_DECLARE          : return TOKEN_DECLARE;
                case RULE_REPRESENTS       : return TOKEN_REPRESENTS;
                case RULE_DEFINE           : return TOKEN_DEFINE;
                case RULE_RETURNS          : return TOKEN_RETURNS;
                case RULE_CAST             : return TOKEN_CAST;

                case RULE_POINTER          : return TOKEN_POINTER;
                case RULE_ADDRESS          : return TOKEN_ADDRESS;
                case RULE_SIZE             : return TOKEN_SIZE;
                case RULE_DEREF            : return TOKEN_DEREF;
                case RULE_DOT              : return TOKEN_DOT;

                case RULE_PLUS             : return TOKEN_PLUS;
                case RULE_MINUS            : return TOKEN_MINUS;
                case RULE_MULTIPLY         : return TOKEN_MULTIPLY;
                case RULE_DIVIDE           : return TOKEN_DIVIDE;
                case RULE_REMAINDER        : return TOKEN_REMAINDER;

                case RULE_COMPLEMENT       : return TOKEN_COMPLEMENT;
                case RULE_BIT_AND          : return TOKEN_BIT_AND;
                case RULE_BIT_OR           : return TOKEN_BIT_OR;
                case RULE_BIT_XOR          : return TOKEN_BIT_XOR;
                case RULE_BIT_RIGHT        : return TOKEN_BIT_RIGHT;
                case RULE_BIT_LEFT         : return TOKEN_BIT_LEFT;

                case RULE_GREATER          : return TOKEN_GREATER;
                case RULE_LESSER           : return TOKEN_LESSER;
                case RULE_GEQ              : return TOKEN_GEQ;
                case RULE_LEQ              : return TOKEN_LEQ;
                case RULE_EQUAL            : return TOKEN_EQUAL;
                case RULE_NOT_EQUAL        : return TOKEN_NOT_EQUAL;
                case RULE_NOT              : return TOKEN_NOT;
                case RULE_LOG_AND          : return TOKEN_LOG_AND;
                case RULE_LOG_OR           : return TOKEN_LOG_OR;

                case RULE_ASSIGN           : return TOKEN_ASSIGN;
                case RULE_ADD_ASSIGN       : return TOKEN_ADD_ASSIGN;
                case RULE_SUB_ASSIGN       : return TOKEN_SUB_ASSIGN;
                case RULE_MUL_ASSIGN       : return TOKEN_MUL_ASSIGN;
                case RULE_DIV_ASSIGN       : return TOKEN_DIV_ASSIGN;
                case RULE_REM_ASSIGN       : return TOKEN_REM_ASSIGN;
                case RULE_AND_ASSIGN       : return TOKEN_AND_ASSIGN;
                case RULE_OR_ASSIGN        : return TOKEN_OR_ASSIGN;
                case RULE_XOR_ASSIGN       : return TOKEN_XOR_ASSIGN;
                case RULE_BIT_RIGHT_ASSIGN : return TOKEN_BIT_RIGHT_ASSIGN;
                case RULE_BIT_LEFT_ASSIGN  : return TOKEN_BIT_LEFT_ASSIGN;

                default                    : return TOKEN_UNDEF;
            }

        }

    }

    void Lexer::read() {

        next_char = file.get();
        if (!file.eof() && (file.fail() || file.bad()) ) {
            input_error = true;
        }

    }

    void Lexer::append() {

        if (token_buffer == "") {
            start = next;
        }
	last = end;
	end = next;

        if (next_char != EOF) {
            token_buffer += char(next_char);
        }

        switch (next_char) {
            case '\n': 
                next.line++; 
                next.column = 1;
                break;
            case '\r': 
                next.column = 1;
                break;
            case '\t':
                next.column += 8;
                break;
            case '\v':
                next.line++;
                break;
            case EOF:
                break;
            default:
                next.column++;
        }
    }

    void Lexer::unappend() {
        token_buffer.erase(token_buffer.end()-1);
        next = end;
	end = last;
    }

    int Lexer::rule() {

        append();

        if (next_char == EOF) {
            token_string = token_buffer = "";
            return (input_error ? RULE_ERROR : RULE_EOF);
        }

        auto m = match(false);
	if (m != RULE_UNDEF) {
            for (; m != RULE_UNDEF; m = match(false)) {
                read();    
                append();
                if (next_char == EOF) {
                    break;
                }
            }
            if (m == RULE_UNDEF) {
                unappend();
                m = match(true);
            }
	    else if (input_error) {
                return RULE_ERROR;
            }
	}

        token_string = token_buffer;
        token_buffer = "";
        state = LEXER_STATE_INIT;
        return m;

    }
}
