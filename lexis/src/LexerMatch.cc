#include <Lexer.h>

namespace dmp {

    int Lexer::match(bool exact) {

        if (token_buffer == "") {
            return RULE_UNDEF;
        }

        switch (state) {
            case LEXER_STATE_INCLUDE :
                return token_buffer.back() != '\n' ? RULE_FILENAME : RULE_UNDEF;
            case LEXER_STATE_WS :
                return isWS(exact) ? RULE_WS : RULE_UNDEF;
            case LEXER_STATE_ONELINE_COMMENT :
                return isOneLineComment(exact) ? RULE_ONELINE_COMMENT : RULE_UNDEF;
            case LEXER_STATE_MULTILINE_COMMENT :
                return isMultiLineComment(exact) ? RULE_MULTILINE_COMMENT : RULE_UNDEF;
            case LEXER_STATE_CHAR :
                return isChar(exact) ? RULE_CHAR : RULE_UNDEF;
            case LEXER_STATE_STRING :
                return isString(exact) ? RULE_STRING : RULE_UNDEF;
            case LEXER_STATE_INT :
                return isInt(exact) ? RULE_INT : RULE_UNDEF;
            case LEXER_STATE_REAL :
                return isReal(exact) ? RULE_REAL : RULE_UNDEF;
            case LEXER_STATE_NUM :
                if (isInt(exact)) {
                    return RULE_INT;
                }
                else if (isReal(exact)) {
                    return RULE_REAL;
                }
                return RULE_UNDEF;
            case LEXER_STATE_WORD :
                for (const auto& keyword_pair : keywords) {
                    const auto& keyword = keyword_pair.second;
                    if (exact) {
                        if (token_buffer == keyword) {
                            return keyword_pair.first;
                        }
                    }
                    else {
                        if (keyword.find(token_buffer) == 0) {
                            return keyword_pair.first;
                        }
                    }
                }
                if (isIdentifier(exact)) {
                    return RULE_IDENT;
                }
                return RULE_UNDEF;
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

        for (const auto& keyword_pair : keywords) {
            const auto& keyword = keyword_pair.second;
            if (exact) {
                if (token_buffer == keyword) {
                    return keyword_pair.first;
                }
            }
            else {
                if (keyword.find(token_buffer) == 0) {
                    state = LEXER_STATE_WORD;
                    return keyword_pair.first;
                }
            }
        }

        if (isIdentifier(exact)) {
            return RULE_IDENT;
        }
        else if (isInt(exact)) {
            return RULE_INT;
        }
        else if (isReal(exact)) {
            return RULE_REAL;
        }
        else if (isChar(exact)) {
            return RULE_CHAR;
        }
        else if (isString(exact)) {
            return RULE_STRING;
        }
        else if (isWS(exact)) {
            return RULE_WS;
        }
        else if (isOneLineComment(exact)) {
            return RULE_ONELINE_COMMENT;
        }
        else if (isMultiLineComment(exact)) {
            return RULE_MULTILINE_COMMENT;
        }

        return RULE_UNDEF;

    }

    bool Lexer::isWS(bool) {

        for (std::size_t i = 0; i < token_buffer.length(); i++) {
            if (token_buffer[i] != ' '  &&
                token_buffer[i] != '\t' &&
                token_buffer[i] != '\n' &&
                token_buffer[i] != '\r' &&
                token_buffer[i] != '\v' &&
                token_buffer[i] != '\f')
            {
                return false;
            }
            if (i == 0) {
                state = LEXER_STATE_WS;
            }
        }

        return true;
    }

    bool Lexer::isOneLineComment(bool exact) {

        if (exact) {
            if (token_buffer.length() < 3) {
                return false;
            }
            if (token_buffer[0] != '`' || token_buffer[1] != '`' || token_buffer[token_buffer.length()-1] != '\n') {
                return false;
            }
            if (token_buffer[token_buffer.length()-1] != '\n') {
                return false;
            }
        }

        else {
            if (token_buffer.length() == 1) {
                if (token_buffer[0] != '`') {
                    return false;
                }
            }
            else if (token_buffer.length() == 2) {
                if (token_buffer[0] != '`' || token_buffer[1] != '`') {
                    return false;
                }
                state = LEXER_STATE_ONELINE_COMMENT;
            }
            else if (state == LEXER_STATE_ONELINE_COMMENT) {
                return *(token_buffer.end()-2) != '\n';
            }
            else {
                return false;
            } 
        }

        return true;
    }

    bool Lexer::isMultiLineComment(bool exact) {

        if (exact) {
            if (token_buffer.length() < 4) {
                return false;
            }   
            if (token_buffer[0] != '/' || token_buffer[1] != '*') {
                return false; 
            }   
            if (token_buffer[token_buffer.length()-2] != '*' || token_buffer[token_buffer.length()-1] != '/') {
                return false;
            }   
        }   

        else { 
            if (token_buffer.length() == 1) {
                if (token_buffer[0] != '/') {
                    return false;
                }
            }
            else if (token_buffer.length() == 2) {
                if (token_buffer[0] != '/' || token_buffer[1] != '*') {
                    return false;
                }
                state = LEXER_STATE_MULTILINE_COMMENT;
            }
            else if (state == LEXER_STATE_MULTILINE_COMMENT) {
                return (*(token_buffer.end()-3) != '*') || (*(token_buffer.end()-2) != '/');
            }
            else {
                return false;
            } 
        }

        return true;
    }   

    bool Lexer::isIdentifier(bool) {

        if (token_buffer[0] != '_' && letter.find(token_buffer[0]) == std::string::npos) {
            return false;
        }   
        
        state = LEXER_STATE_WORD;

        for (std::size_t i = 1; i < token_buffer.length(); i++) {
            if (token_buffer[i] != '_' && letter.find(token_buffer[i]) == std::string::npos && dec.find(token_buffer[i])  == std::string::npos) {
                return false;
            }
        }   
            
        return true;
    }

    bool Lexer::isInt(bool exact) {

        int base = 0;

        if (dec.find(token_buffer[0]) == std::string::npos) {
            return false;
        }

        state = LEXER_STATE_NUM;

        if (token_buffer.length() == 1) {
            return true;
        }

        if (dec.find(token_buffer[1]) != std::string::npos) {
            base = 10;
        }
        if (token_buffer[0] == '0') {
            switch (token_buffer[1]) {
                case 'b' : base =  2; state = LEXER_STATE_INT; break;
                case 'o' : base =  8; state = LEXER_STATE_INT; break;
                case 'x' : base = 16; state = LEXER_STATE_INT; break;
            }
        }

        if (base == 0) {
            return false;
        }

        if (token_buffer.length() < 3) {
            return (base == 10 ? true : !exact);
        }

        for (std::size_t i = 2; i < token_buffer.length(); i++) {
            if (base ==  2 && bin.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
            if (base ==  8 && oct.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
            if (base == 10 && dec.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
            if (base == 16 && hex.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
        }

        return true;

    }

    bool Lexer::isReal(bool exact) {

        if (token_buffer == ".") {
            return !exact;
        }

        if (token_buffer[0] != '.' && dec.find(token_buffer[0]) == std::string::npos) {
            return false;
        }

        if (token_buffer.length() > 1) {
            if (token_buffer[0] == '.' && dec.find(token_buffer[0]) == std::string::npos) {
                return false;
            }
        }

        state = LEXER_STATE_NUM;

        std::size_t pos = 0;
        while (pos < token_buffer.length() && dec.find(token_buffer[pos]) != std::string::npos) {
            pos++;
        }
        if (pos == token_buffer.length()) {
            return !exact;
        }

        if (token_buffer[pos] == '.') {

            state = LEXER_STATE_REAL;

            pos++;
            while (pos < token_buffer.length() && dec.find(token_buffer[pos]) != std::string::npos) {
                pos++;
            }
            if (pos == token_buffer.length()) {
                return true;
            }
        }

        if (token_buffer[pos] != 'e' && token_buffer[pos] != 'E') {
            return false;
        }

        state = LEXER_STATE_REAL;

        pos++;
        if (pos == token_buffer.length()) {
            return !exact;
        }
        if (token_buffer[pos] == '+' || token_buffer[pos] == '-') {
            pos++;
        }
        if (pos == token_buffer.length()) {
            return !exact;
        }
        while (pos < token_buffer.length()) {
            if (dec.find(token_buffer[pos]) == std::string::npos) {
                return false;
            }
        }
        return true;
    }

    bool Lexer::isChar(bool exact) {

        if (token_buffer[0] != '\'') {
            return false;
        }

        state = LEXER_STATE_CHAR;

        if (token_buffer.length() == 1) {
            return !exact;
        }

        std::size_t i = 1;
        while (i < token_buffer.length()) {
            if (!nextChar(i, exact)) {
                return false;
            }
            i++;
            if (i == token_buffer.length()) {
                return !exact;
            }
            else if (i == token_buffer.length()-1) {
                return (token_buffer[i] == '\'') || !exact;
            }
        }

        return false;
    }

    bool Lexer::isString(bool exact) {

        if (token_buffer[0] != '\"') {
            return false;
        }

        state = LEXER_STATE_STRING;

        if (token_buffer.length() == 1) {
            return !exact;
        }

        if (token_buffer == "\"\"") {
            return true;
        }

        std::size_t i = 1;
        while (i < token_buffer.length()) {
            if (!nextChar(i, exact)) {
                return false;
            }
            i++;
            if (i == token_buffer.length()) {
                return !exact;
            }
            else if (i == token_buffer.length()-1) {
                return (token_buffer[i] == '\"') || !exact;
            }
        }

        return false;
    }

    bool Lexer::nextChar(std::size_t& i, bool exact) {

        if (i >= token_buffer.length()) {
            return false;
        }
        if (token_buffer[i] == '\'' || 
            token_buffer[i] == '\"' ||
            token_buffer[i] == '\f' || 
            token_buffer[i] == '\n' ||
            token_buffer[i] == '\r' ||
            token_buffer[i] == '\v') 
        {
            return false;
        }


        if (token_buffer[i] != '\\') {
            return true;
        }

        if (i == token_buffer.length()-1) {
            return !exact;
        }
        i++;
        if (token_buffer[i] == 'a'  ||
            token_buffer[i] == 'b'  ||
            token_buffer[i] == 'f'  ||
            token_buffer[i] == 'n'  ||
            token_buffer[i] == 'r'  ||
            token_buffer[i] == 't'  ||
            token_buffer[i] == 'v'  ||
            token_buffer[i] == '\\' ||
            token_buffer[i] == '\'' ||
            token_buffer[i] == '\"')
        {   
            return true;
        }
        else if (token_buffer[i] == '0' ||
                 token_buffer[i] == '1' ||
                 token_buffer[i] == '2' ||
                 token_buffer[i] == '3' ||
                 token_buffer[i] == 'x')
        {
            const auto& digits = (token_buffer[i] == 'x' ? hex : oct);
            if (i == token_buffer.length()-1) {
                return !exact;
            }
            i++;
            if (digits.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
            if (i == token_buffer.length()-1) {
                return !exact;
            }
            i++;
            if (digits.find(token_buffer[i]) == std::string::npos) {
                return false;
            }
            return true;
        }
        return false;
    }

}
