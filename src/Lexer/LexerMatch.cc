#include <Lexer/Lexer.h>

namespace dmp {

    static int process_LEXER_STATE_INIT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_INCLUDE(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_WS(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_ONELINE_COMMENT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_MULTILINE_COMMENT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_CHAR_START(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_CHAR_NEXT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_CHAR_ESC(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_CHAR_ESC_HEX(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_CHAR_DONE(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_STRING_START(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_STRING_NEXT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_STRING_ESC(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_STRING_ESC_HEX(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_STRING_DONE(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_NUM(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_INT_BIN(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_INT_OCT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_INT_HEX(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_REAL_DOT(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_REAL_EXP(bool exact, const std::string& token_buffer, int& state);
    static int process_LEXER_STATE_WORD(bool exact, const std::string& token_buffer, int& state);

    static inline int select_rule(bool selector, int rule_if_true) {
        return selector ? rule_if_true : RULE_UNDEF;
    }

    void Lexer::populate_state_processors() {
        state_processors[LEXER_STATE_INIT]              = process_LEXER_STATE_INIT;
        state_processors[LEXER_STATE_INCLUDE]           = process_LEXER_STATE_INCLUDE;
        state_processors[LEXER_STATE_WS]                = process_LEXER_STATE_WS;
        state_processors[LEXER_STATE_ONELINE_COMMENT]   = process_LEXER_STATE_ONELINE_COMMENT;
        state_processors[LEXER_STATE_MULTILINE_COMMENT] = process_LEXER_STATE_MULTILINE_COMMENT;
        state_processors[LEXER_STATE_CHAR_START]        = process_LEXER_STATE_CHAR_START;
        state_processors[LEXER_STATE_CHAR_NEXT]         = process_LEXER_STATE_CHAR_NEXT;
        state_processors[LEXER_STATE_CHAR_ESC]          = process_LEXER_STATE_CHAR_ESC;
        state_processors[LEXER_STATE_CHAR_ESC_HEX]      = process_LEXER_STATE_CHAR_ESC_HEX;
        state_processors[LEXER_STATE_CHAR_DONE]         = process_LEXER_STATE_CHAR_DONE;
        state_processors[LEXER_STATE_STRING_START]      = process_LEXER_STATE_STRING_START;
        state_processors[LEXER_STATE_STRING_NEXT]       = process_LEXER_STATE_STRING_NEXT;
        state_processors[LEXER_STATE_STRING_ESC]        = process_LEXER_STATE_STRING_ESC;
        state_processors[LEXER_STATE_STRING_ESC_HEX]    = process_LEXER_STATE_STRING_ESC_HEX;
        state_processors[LEXER_STATE_STRING_DONE]       = process_LEXER_STATE_STRING_DONE;
        state_processors[LEXER_STATE_NUM]               = process_LEXER_STATE_NUM;
        state_processors[LEXER_STATE_INT_BIN]           = process_LEXER_STATE_INT_BIN;
        state_processors[LEXER_STATE_INT_OCT]           = process_LEXER_STATE_INT_OCT;
        state_processors[LEXER_STATE_INT_HEX]           = process_LEXER_STATE_INT_HEX;
        state_processors[LEXER_STATE_REAL_DOT]          = process_LEXER_STATE_REAL_DOT;
        state_processors[LEXER_STATE_REAL_EXP]          = process_LEXER_STATE_REAL_EXP;
        state_processors[LEXER_STATE_WORD]              = process_LEXER_STATE_WORD;
    }

    int Lexer::match(bool exact) {

        if (token_buffer == "") {
            return RULE_UNDEF;
        }

	return state_processors[state](exact, token_buffer, state);

    }

    int process_LEXER_STATE_INIT(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

	if (token_buffer.length() == 1) {
            bool is_white_space = last_ch == ' '  ||
                                  last_ch == '\t' ||
                                  last_ch == '\n' ||
                                  last_ch == '\r' ||
                                  last_ch == '\v' ||
                                  last_ch == '\f';
            if (is_white_space) {
                state = LEXER_STATE_WS;
		return RULE_WS;
            }
	    else if (Lexer::letter.find(token_buffer[0]) != std::string::npos || token_buffer[0] == '_') {
                state = LEXER_STATE_WORD;
		// We don't have any 1-char keywords
		// If there are any, they should be tested here
		return RULE_IDENT;
            }
	    else if (last_ch == '`') {
                state = LEXER_STATE_ONELINE_COMMENT;
                return exact ? RULE_UNDEF : RULE_ONELINE_COMMENT;
            }
	    else if (Lexer::dec.find(last_ch) != std::string::npos) {
                state = LEXER_STATE_NUM;
                return RULE_INT;
            }
	    else if (last_ch == '\'') {
                state = LEXER_STATE_CHAR_START;
                return exact ? RULE_UNDEF : RULE_CHAR;
            }
	    else if (last_ch == '\"') {
                state = LEXER_STATE_STRING_START;
                return exact ? RULE_UNDEF : RULE_STRING;
            }
        }

	else if (token_buffer.length() == 2) {
            if (token_buffer[0] == '/' && last_ch == '*') {
                state = LEXER_STATE_MULTILINE_COMMENT;
                return exact ? RULE_UNDEF : RULE_MULTILINE_COMMENT;
            }
	    else if (token_buffer[0] == '.' && Lexer::dec.find(last_ch) != std::string::npos) {
                state = LEXER_STATE_REAL_DOT;
                return exact ? RULE_UNDEF : RULE_REAL;
            }
        }

	for (const auto& sym_pair : Lexer::symbols) {
            const auto& sym = sym_pair.second;
            if (exact) {
                if (token_buffer == sym) {
                    return sym_pair.first;
                }
            }
            else {
                if (sym.find(token_buffer) == 0) {
                    return sym_pair.first;
                }
            }
        }

	return RULE_UNDEF;
    }

    int process_LEXER_STATE_INCLUDE(bool exact, const std::string& token_buffer, int& state) {
        return token_buffer.back() != '\n' ? RULE_FILENAME : RULE_UNDEF;
    }

    int process_LEXER_STATE_WS(bool, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();
        bool is_white_space = last_ch == ' '  ||
                              last_ch == '\t' ||
                              last_ch == '\n' ||
                              last_ch == '\r' ||
                              last_ch == '\v' ||
                              last_ch == '\f';
        return select_rule(is_white_space, RULE_WS);
    }

    int process_LEXER_STATE_WORD(bool exact, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();

        if (exact) {
            for (const auto& keyword_pair : Lexer::keywords) {
                const auto& keyword = keyword_pair.second;
                if (token_buffer == keyword) {
                    return keyword_pair.first;
                }
            }
        }

        bool is_ident = Lexer::letter.find(last_ch) != std::string::npos ||
                        Lexer::dec.find(last_ch)    != std::string::npos ||
                        last_ch == '_';

        return select_rule(is_ident, RULE_IDENT);
    }

    int process_LEXER_STATE_ONELINE_COMMENT(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (len == 2) {
            if (last_ch == '`') {
                return select_rule(!exact, RULE_ONELINE_COMMENT);
            }
        }
        else {
            if (exact) {
                return select_rule(last_ch == '\n', RULE_ONELINE_COMMENT);
            }
            else {
                return select_rule(token_buffer[len-2] != '\n', RULE_ONELINE_COMMENT);
            }
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_MULTILINE_COMMENT(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (len == 3) {
            return select_rule(!exact, RULE_MULTILINE_COMMENT);
        }
        else {
            if (exact) {
                return select_rule(token_buffer[len-2] == '*' && last_ch == '/', RULE_MULTILINE_COMMENT);
            }
            else {
                return select_rule(token_buffer[len-3] != '*' || token_buffer[len-2] != '/', RULE_MULTILINE_COMMENT);
            }
        }
        return RULE_UNDEF;	    
    }   

    int process_LEXER_STATE_NUM(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (len == 2 && token_buffer[0] == '0') {
            switch (last_ch) {
                case 'b' : state = LEXER_STATE_INT_BIN ; return select_rule(!exact, RULE_INT);
                case 'o' : state = LEXER_STATE_INT_OCT ; return select_rule(!exact, RULE_INT);
                case 'x' : state = LEXER_STATE_INT_HEX ; return select_rule(!exact, RULE_INT);
                case '.' : state = LEXER_STATE_REAL_DOT; return select_rule(!exact, RULE_REAL);
            }
	    return select_rule(Lexer::dec.find(last_ch) != std::string::npos, RULE_INT);
        }
        else if (Lexer::dec.find(last_ch) != std::string::npos) {
            return RULE_INT;
        } 
	else if (last_ch == '.') {
            state = LEXER_STATE_REAL_DOT;
            return RULE_REAL;
        }
        else if (last_ch == 'e' || last_ch == 'E') {
            state = LEXER_STATE_REAL_EXP;
            return select_rule(!exact, RULE_REAL);
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_INT_BIN(bool exact, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();
        return select_rule(Lexer::bin.find(last_ch) != std::string::npos, RULE_INT);
    }

    int process_LEXER_STATE_INT_OCT(bool exact, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();
        return select_rule(Lexer::oct.find(last_ch) != std::string::npos, RULE_INT);
    }

    int process_LEXER_STATE_INT_HEX(bool exact, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();
        return select_rule(Lexer::hex.find(last_ch) != std::string::npos, RULE_INT);
    }    

    int process_LEXER_STATE_REAL_DOT(bool exact, const std::string& token_buffer, int& state) {

        const auto& last_ch = token_buffer.back();
	if (last_ch == 'e' || last_ch == 'E') {
            state = LEXER_STATE_REAL_EXP;
	    return select_rule(!exact, RULE_REAL);
        }
	else if (Lexer::dec.find(last_ch) != std::string::npos) {
            return RULE_REAL;
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_REAL_EXP(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (last_ch == '+' || last_ch == '-') {
            if (token_buffer[len-2] == 'e' || token_buffer[len-2] == 'E') {
                return select_rule(!exact, RULE_REAL);
            }
        }
        else if (Lexer::dec.find(last_ch) != std::string::npos) {
            return RULE_REAL;
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_CHAR_START(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (Lexer::charset.find(last_ch) != std::string::npos) {
            state = LEXER_STATE_CHAR_NEXT;
            return select_rule(!exact, RULE_CHAR);
        }
        else if (last_ch == '\\') {
            state = LEXER_STATE_CHAR_ESC;
            return select_rule(!exact, RULE_CHAR);
        }
	return RULE_UNDEF;
    }

    int process_LEXER_STATE_CHAR_NEXT(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (last_ch == '\'') {
            state = LEXER_STATE_CHAR_DONE;
            return RULE_CHAR;
        }
	return RULE_UNDEF;
    }

    int process_LEXER_STATE_CHAR_ESC(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (last_ch == 'a'  ||
            last_ch == 'b'  ||
            last_ch == 'f'  ||
            last_ch == 'n'  ||
            last_ch == 'r'  ||
            last_ch == 't'  ||
            last_ch == 'v'  ||
            last_ch == '\\' ||
            last_ch == '\'' ||
            last_ch == '\"')
        {
            state = LEXER_STATE_CHAR_NEXT;
            return select_rule(!exact, RULE_CHAR);
        }
        else if (last_ch == 'x') {
            state = LEXER_STATE_CHAR_ESC_HEX;
            return select_rule(!exact, RULE_CHAR);
        }
        return RULE_UNDEF;	    
    }

    int process_LEXER_STATE_CHAR_ESC_HEX(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (Lexer::hex.find(last_ch)) {
            if (token_buffer[len-2] == 'x') {
                return select_rule(!exact, RULE_CHAR);
            }
            else {
                state = LEXER_STATE_CHAR_NEXT;
                return select_rule(!exact, RULE_CHAR);
            }
        }
        return RULE_UNDEF;	    
    }

    int process_LEXER_STATE_CHAR_DONE(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();
        return select_rule(exact, RULE_CHAR);
    }

    int process_LEXER_STATE_STRING_START(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (Lexer::charset.find(last_ch) != std::string::npos) {
            state = LEXER_STATE_STRING_NEXT;
            return select_rule(!exact, RULE_STRING);
        }
        else if (last_ch == '\\') {
            state = LEXER_STATE_STRING_ESC;
            return select_rule(!exact, RULE_STRING);
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_STRING_NEXT(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (last_ch == '\"') {
            state = LEXER_STATE_STRING_DONE;
            return RULE_STRING;
        }
        else if (Lexer::charset.find(last_ch) != std::string::npos) {
            return select_rule(!exact, RULE_STRING);
        }
        else if (last_ch == '\\') {
            state = LEXER_STATE_STRING_ESC;
            return select_rule(!exact, RULE_STRING);
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_STRING_ESC(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (last_ch == 'a'  ||
            last_ch == 'b'  ||
            last_ch == 'f'  ||
            last_ch == 'n'  ||
            last_ch == 'r'  ||
            last_ch == 't'  ||
            last_ch == 'v'  ||
            last_ch == '\\' ||
            last_ch == '\'' ||
            last_ch == '\"')
        {
            state = LEXER_STATE_STRING_NEXT;
            return select_rule(!exact, RULE_STRING);
        }
        else if (last_ch == 'x') {
            state = LEXER_STATE_STRING_ESC_HEX;
            return select_rule(!exact, RULE_STRING);
        }
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_STRING_ESC_HEX(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (Lexer::hex.find(last_ch)) {
            if (token_buffer[len-2] == 'x') {
                return select_rule(!exact, RULE_STRING);
            }
            else {
                state = LEXER_STATE_STRING_NEXT;
                return select_rule(!exact, RULE_STRING);
            }
        }	
        return RULE_UNDEF;
    }

    int process_LEXER_STATE_STRING_DONE(bool exact, const std::string& token_buffer, int& state) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();
        return select_rule(exact, RULE_STRING);
    }

}
