#include <Lexer/Lexer.h>

namespace dmp {

    int Lexer::match(bool exact) {

        if (token_buffer == "") {
            return RULE_UNDEF;
        }
        else if (state == LEXER_STATE_INCLUDE) {
            return token_buffer.back() != '\n' ? RULE_FILENAME : RULE_UNDEF;
        }
	else if (state == LEXER_STATE_WS) {
            return isWS(exact) ? RULE_WS : RULE_UNDEF;
        }
	else if (state == LEXER_STATE_ONELINE_COMMENT) {
            return isOneLineComment(exact) ? RULE_ONELINE_COMMENT : RULE_UNDEF;
        }
	else if (state == LEXER_STATE_MULTILINE_COMMENT) {
            return isMultiLineComment(exact) ? RULE_MULTILINE_COMMENT : RULE_UNDEF;
        }
	else if (state == LEXER_STATE_CHAR_START   ||
                 state == LEXER_STATE_CHAR_NEXT    ||
                 state == LEXER_STATE_CHAR_ESC     ||
                 state == LEXER_STATE_CHAR_ESC_HEX ||
                 state == LEXER_STATE_CHAR_DONE)
        {
            return isChar(exact) ? RULE_CHAR : RULE_UNDEF;
        }
        else if (state == LEXER_STATE_STRING_START   ||
                 state == LEXER_STATE_STRING_NEXT    ||
                 state == LEXER_STATE_STRING_ESC     ||
                 state == LEXER_STATE_STRING_ESC_HEX ||
                 state == LEXER_STATE_STRING_DONE)
        {
            return isString(exact) ? RULE_STRING : RULE_UNDEF;
        }
        else if (state == LEXER_STATE_INT_BIN ||
                 state == LEXER_STATE_INT_OCT ||
                 state == LEXER_STATE_INT_HEX)
        {
            return isInt(exact) ? RULE_INT : RULE_UNDEF;
        }
        else if (state == LEXER_STATE_REAL_DOT ||
                 state == LEXER_STATE_REAL_EXP)
        {
            return isReal(exact) ? RULE_REAL : RULE_UNDEF;
        }
        else if (state == LEXER_STATE_NUM) {
            if (isInt(exact)) {
                return RULE_INT;
            }
            else if (isReal(exact)) {
                return RULE_REAL;
            }
            return RULE_UNDEF;
        }
	else if (state == LEXER_STATE_WORD) {
            if (exact) {
                for (const auto& keyword_pair : keywords) {
                    const auto& keyword = keyword_pair.second;
                    if (token_buffer == keyword) {
                        return keyword_pair.first;
                    }
                }
            }
            if (isIdentifier(exact)) {
                return RULE_IDENT;
            }
            return RULE_UNDEF;
        }

        return takeFirstLook(exact);

    }

    int Lexer::takeFirstLook(bool exact) {

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
	    else if (letter.find(token_buffer[0]) != std::string::npos || token_buffer[0] == '_') {
                state = LEXER_STATE_WORD;
		// We don't have any 1-char keywords
		// If there are any, they should be tested here
		return RULE_IDENT;
            }
	    else if (last_ch == '`') {
                state = LEXER_STATE_ONELINE_COMMENT;
                return exact ? RULE_UNDEF : RULE_ONELINE_COMMENT;
            }
	    else if (dec.find(last_ch) != std::string::npos) {
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
	    else if (token_buffer[0] == '.' && dec.find(last_ch) != std::string::npos) {
                state = LEXER_STATE_REAL_DOT;
                return exact ? RULE_UNDEF : RULE_REAL;
            }
        }

	for (const auto& sym_pair : symbols) {
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

    bool Lexer::isWS(bool) {

        const auto& last_ch = token_buffer.back();
	return last_ch == ' '  ||
               last_ch == '\t' ||
               last_ch == '\n' ||
               last_ch == '\r' ||
               last_ch == '\v' ||
               last_ch == '\f';
    }

    bool Lexer::isIdentifier(bool) {

        const auto& last_ch = token_buffer.back();
        return letter.find(last_ch) != std::string::npos ||
               dec.find(last_ch)    != std::string::npos ||
               last_ch == '_';	
    }

    bool Lexer::isOneLineComment(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (len == 2) {
            if (last_ch == '`') {
                return !exact;
            }
        }
        else {
            if (exact) {
                return last_ch == '\n';
            }
            else {
                return token_buffer[len-2] != '\n';
            }
        }
        return false;
    }

    bool Lexer::isMultiLineComment(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (len == 3) {
            return !exact;
        }
        else {
            if (exact) {
                return token_buffer[len-2] == '*' && last_ch == '/';
            }
            else {
                return token_buffer[len-3] != '*' || token_buffer[len-2] != '/';
            }
        }
        return false;	    
    }   

    bool Lexer::isInt(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

	if (state == LEXER_STATE_NUM) {
            if (len == 2 && token_buffer[0] == '0') {
                switch (last_ch) {
                    case 'b' : state = LEXER_STATE_INT_BIN; return !exact;
                    case 'o' : state = LEXER_STATE_INT_OCT; return !exact;
                    case 'x' : state = LEXER_STATE_INT_HEX; return !exact;
                    default  : return dec.find(last_ch) != std::string::npos;
                }
            }
	    return dec.find(last_ch) != std::string::npos;
        }
	else if (state == LEXER_STATE_INT_BIN) {
            return bin.find(last_ch) != std::string::npos;
        }
	else if (state == LEXER_STATE_INT_OCT) {
            return oct.find(last_ch) != std::string::npos;
        }
	else if (state == LEXER_STATE_INT_HEX) {
            return hex.find(last_ch) != std::string::npos;
        }
        return false;
    }

    bool Lexer::isReal(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

	if (len >= 2 && state == LEXER_STATE_NUM) {
            if (last_ch == '.') {
                state = LEXER_STATE_REAL_DOT;
		return true;
            }
	    else if (last_ch == 'e' || last_ch == 'E') {
                state = LEXER_STATE_REAL_EXP;
		return !exact;
            }
	    else if (dec.find(last_ch) != std::string::npos) {
                return true;
            }
        }
	else if (state == LEXER_STATE_REAL_DOT) {
	    if (last_ch == 'e' || last_ch == 'E') {
                state = LEXER_STATE_REAL_EXP;
		return !exact;
            }
	    else if (dec.find(last_ch) != std::string::npos) {
                return true;
            }
        }
	else if (state == LEXER_STATE_REAL_EXP) {
            if (last_ch == '+' || last_ch == '-') {
                if (token_buffer[len-2] == 'e' || token_buffer[len-2] == 'E') {
                    return !exact;
                }
            }
	    else if (dec.find(last_ch) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool Lexer::isChar(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

	if (state == LEXER_STATE_CHAR_START) {
            if (charset.find(last_ch) != std::string::npos) {
                state = LEXER_STATE_CHAR_NEXT;
                return !exact;
            }
	    else if (last_ch == '\\') {
                state = LEXER_STATE_CHAR_ESC;
		return !exact;
            }
        }
	else if (state == LEXER_STATE_CHAR_NEXT) {
            if (last_ch == '\'') {
                state = LEXER_STATE_CHAR_DONE;
                return true;
            }
        }
	else if (state == LEXER_STATE_CHAR_ESC) {
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
                return !exact;
            }
	    else if (last_ch == 'x') {
                state = LEXER_STATE_CHAR_ESC_HEX;
                return !exact;
            }
        }
	else if (state == LEXER_STATE_CHAR_ESC_HEX) {
            if (hex.find(last_ch)) {
                if (token_buffer[len-2] == 'x') {
                    return !exact;
                }
		else {
                    state = LEXER_STATE_CHAR_NEXT;
                    return !exact;
                }
	    }
        }
	else if (state == LEXER_STATE_CHAR_DONE) {
            return exact;
        }
	return false;
    }

    bool Lexer::isString(bool exact) {

        auto len = token_buffer.length();
        const auto& last_ch = token_buffer.back();

        if (state == LEXER_STATE_STRING_START) {
            if (charset.find(last_ch) != std::string::npos) {
                state = LEXER_STATE_STRING_NEXT;
                return !exact;
            }
            else if (last_ch == '\\') {
                state = LEXER_STATE_STRING_ESC;
                return !exact;
            }
        }
        else if (state == LEXER_STATE_STRING_NEXT) {
            if (last_ch == '\"') {
                state = LEXER_STATE_STRING_DONE;
                return true;
            }
	    else if (charset.find(last_ch) != std::string::npos) {
                return !exact;
            }
            else if (last_ch == '\\') {
                state = LEXER_STATE_STRING_ESC;
                return !exact;
            }
        }
        else if (state == LEXER_STATE_STRING_ESC) {
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
                return !exact;
            }
            else if (last_ch == 'x') {
                state = LEXER_STATE_STRING_ESC_HEX;
                return !exact;
            }
        }
        else if (state == LEXER_STATE_STRING_ESC_HEX) {
            if (hex.find(last_ch)) {
                if (token_buffer[len-2] == 'x') {
                    return !exact;
                }
                else {
                    state = LEXER_STATE_STRING_NEXT;
                    return !exact;
                }
            }
        }
        else if (state == LEXER_STATE_STRING_DONE) {
            return exact;
        }
        return false;	
    }
}
