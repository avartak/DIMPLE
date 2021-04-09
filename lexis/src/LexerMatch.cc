#include <Lexer.h>

namespace avl {

    std::shared_ptr<int> Lexer::match(bool exact) {

        std::shared_ptr<int> m;

        if (token == "") {
            return m;
        }

        switch (state) {
            case LEXER_STATE_INCL :
                return token.back() != '\n' ? std::make_shared<int>(RULE_FILENAME) : nullptr;
            case LEXER_STATE_WS :
                return isWS(exact) ? std::make_shared<int>(RULE_WS) : nullptr;
            case LEXER_STATE_ONELINE_COMMENT :
                return isOneLineComment(exact) ? std::make_shared<int>(RULE_ONELINE_COMMENT) : nullptr;
            case LEXER_STATE_MULTILINE_COMMENT :
                return isMultiLineComment(exact) ? std::make_shared<int>(RULE_MULTILINE_COMMENT) : nullptr;
            case LEXER_STATE_CHAR :
                return isChar(exact) ? std::make_shared<int>(RULE_CHAR) : nullptr;
            case LEXER_STATE_STRING :
                return isString(exact) ? std::make_shared<int>(RULE_STRING) : nullptr;
            case LEXER_STATE_INT :
                return isInt(exact) ? std::make_shared<int>(RULE_INT) : nullptr;
            case LEXER_STATE_REAL :
                return isReal(exact) ? std::make_shared<int>(RULE_REAL) : nullptr;
            case LEXER_STATE_NUM :
                if (isInt(exact)) {
                    return std::make_shared<int>(RULE_INT);
                }
                else if (isReal(exact)) {
                    return std::make_shared<int>(RULE_REAL);
                }
                return nullptr;
            case LEXER_STATE_WORD :
                for (const auto& keyword_pair : keywords) {
                    const auto& keyword = keyword_pair.second;
                    if (exact) {
                        if (token == keyword) {
                            return std::make_shared<int>(keyword_pair.first);
                        }
                    }
                    else {
                        if (keyword.find(token) == 0) {
                            return std::make_shared<int>(keyword_pair.first);
                        }
                    }
                }
                if (isIdentifier(exact)) {
                    return std::make_shared<int>(RULE_IDENT);
                }
                return nullptr;
        }

        for (const auto& sym_pair : symbols) {
            const auto& sym = sym_pair.second;
            if (exact) {
                if (token == sym) {
                    return std::make_shared<int>(sym_pair.first);
                }
            }
            else {
                if (sym.find(token) == 0) {
                    return std::make_shared<int>(sym_pair.first);
                }
            }
        }

        for (const auto& keyword_pair : keywords) {
            const auto& keyword = keyword_pair.second;
            if (exact) {
                if (token == keyword) {
                    return std::make_shared<int>(keyword_pair.first);
                }
            }
            else {
                if (keyword.find(token) == 0) {
                    state = LEXER_STATE_WORD;
                    return std::make_shared<int>(keyword_pair.first);
                }
            }
        }

        if (isIdentifier(exact)) {
            return std::make_shared<int>(RULE_IDENT);
        }
        else if (isInt(exact)) {
            return std::make_shared<int>(RULE_INT);
        }
        else if (isReal(exact)) {
            return std::make_shared<int>(RULE_REAL);
        }
        else if (isChar(exact)) {
            return std::make_shared<int>(RULE_CHAR);
        }
        else if (isString(exact)) {
            return std::make_shared<int>(RULE_STRING);
        }
        else if (isWS(exact)) {
            return std::make_shared<int>(RULE_WS);
        }
        else if (isOneLineComment(exact)) {
            return std::make_shared<int>(RULE_ONELINE_COMMENT);
        }
        else if (isMultiLineComment(exact)) {
            return std::make_shared<int>(RULE_MULTILINE_COMMENT);
        }

        return m;

    }

    bool Lexer::isWS(bool) {

        for (std::size_t i = 0; i < token.length(); i++) {
            if (token[i] != ' '  &&
                token[i] != '\t' &&
                token[i] != '\n' &&
                token[i] != '\r' &&
                token[i] != '\v' &&
                token[i] != '\f')
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
            if (token.length() < 3) {
                return false;
            }
    
            if (token[0] != '`' || token[1] != '`' || token[token.length()-1] != '\n') {
                return false;
            }
        }

        else {
            if (token.length() == 1) {
                if (token[0] != '`') {
                    return false;
                }
            }
            else if (token.length() == 2) {
                if (token[0] != '`' || token[1] != '`') {
                    return false;
                }
                state = LEXER_STATE_ONELINE_COMMENT;
            }
            else if (state == LEXER_STATE_ONELINE_COMMENT) {
                return *(token.end()-2) != '\n';
            }
            else {
                return false;
            } 
        }

        return true;
    }

    bool Lexer::isMultiLineComment(bool exact) {

        if (exact) {
            if (token.length() < 4) {
                return false;
            }   
            
            if (token[0] != '/' || token[1] != '*') {
                return false; 
            }   
            if (token[token.length()-2] != '*' || token[token.length()-1] != '/') {
                return false;
            }   
        }   

        else { 
            if (token.length() == 1) {
                if (token[0] != '/') {
                    return false;
                }
            }
            else if (token.length() == 2) {
                if (token[0] != '/' || token[1] != '*') {
                    return false;
                }
                state = LEXER_STATE_MULTILINE_COMMENT;
            }
            else if (state == LEXER_STATE_MULTILINE_COMMENT) {
                if (token.length() < 3) {
                    return !exact;
                }
                return (*(token.end()-2) != '/') || (*(token.end()-3) != '*');
            }
            else {
                return false;
            } 
        }

        for (std::size_t i = 2; i < token.length()-1; i++) {
            if (token[i] == '*' && token[i+1] == '/') {
                if (i != token.length()-2) {
                    return false;
                }   
            }   
        }
        
        return true;
    }   

    bool Lexer::isIdentifier(bool) {

        if (token[0] != '_' && letter.find(token[0]) == std::string::npos) {
            return false;
        }   
        
        state = LEXER_STATE_WORD;

        for (std::size_t i = 1; i < token.length(); i++) {
            if (token[i] != '_' && letter.find(token[i]) == std::string::npos && dec.find(token[i])  == std::string::npos) {
                return false;
            }
        }   
            
        return true;
    }

    bool Lexer::isInt(bool exact) {

        int base = 0;

        if (dec.find(token[0]) == std::string::npos) {
            return false;
        }

        state = LEXER_STATE_NUM;

        if (token.length() == 1) {
            return true;
        }

        if (dec.find(token[1]) != std::string::npos) {
            base = 10;
        }
        if (token[0] == '0') {
            switch (token[1]) {
                case 'b' : base =  2; state = LEXER_STATE_INT; break;
                case 'o' : base =  8; state = LEXER_STATE_INT; break;
                case 'x' : base = 16; state = LEXER_STATE_INT; break;
            }
        }

        if (base == 0) {
            return false;
        }

        if (token.length() < 3) {
            return (base == 10 ? true : !exact);
        }

        for (std::size_t i = 2; i < token.length(); i++) {
            if (base ==  2 && bin.find(token[i]) == std::string::npos) {
                return false;
            }
            if (base ==  8 && oct.find(token[i]) == std::string::npos) {
                return false;
            }
            if (base == 10 && dec.find(token[i]) == std::string::npos) {
                return false;
            }
            if (base == 16 && hex.find(token[i]) == std::string::npos) {
                return false;
            }
        }

        return true;

    }

    bool Lexer::isReal(bool exact) {

        if (token == ".") {
            return !exact;
        }

        if (token[0] != '.' && dec.find(token[0]) == std::string::npos) {
            return false;
        }

        if (token.length() > 1) {
            if (token[0] == '.' && dec.find(token[0]) == std::string::npos) {
                return false;
            }
        }

        state = LEXER_STATE_NUM;

        std::size_t pos = 0;
        while (pos < token.length() && dec.find(token[pos]) != std::string::npos) {
            pos++;
        }
        if (pos == token.length()) {
            return !exact;
        }

        if (token[pos] == '.') {

            state = LEXER_STATE_REAL;

            pos++;
            while (pos < token.length() && dec.find(token[pos]) != std::string::npos) {
                pos++;
            }
            if (pos == token.length()) {
                return true;
            }
        }

        if (token[pos] != 'e' && token[pos] != 'E') {
            return false;
        }

        state = LEXER_STATE_REAL;

        pos++;
        if (pos == token.length()) {
            return !exact;
        }
        if (token[pos] == '+' || token[pos] == '-') {
            pos++;
        }
        if (pos == token.length()) {
            return !exact;
        }
        while (pos < token.length()) {
            if (dec.find(token[pos]) == std::string::npos) {
                return false;
            }
        }
        return true;
    }

    bool Lexer::isChar(bool exact) {

        if (token[0] != '\'') {
            return false;
        }

        state = LEXER_STATE_CHAR;

        if (token.length() == 1) {
            return !exact;
        }

        std::size_t i = 1;
        while (i < token.length()) {
            if (!nextChar(i, exact)) {
                return false;
            }
            i++;
            if (i == token.length()) {
                return !exact;
            }
            else if (i == token.length()-1) {
                return (token[i] == '\'') || !exact;
            }
        }

        return false;
    }

    bool Lexer::isString(bool exact) {

        if (token[0] != '\"') {
            return false;
        }

        state = LEXER_STATE_STRING;

        if (token.length() == 1) {
            return !exact;
        }

        if (token == "\"\"") {
            return true;
        }

        std::size_t i = 1;
        while (i < token.length()) {
            if (!nextChar(i, exact)) {
                return false;
            }
            i++;
            if (i == token.length()) {
                return !exact;
            }
            else if (i == token.length()-1) {
                return (token[i] == '\"') || !exact;
            }
        }

        return false;
    }

    bool Lexer::nextChar(std::size_t& i, bool exact) {

        if (i >= token.length()) {
            return false;
        }
        if (token[i] == '\'' || 
            token[i] == '\"' ||
            token[i] == '\f' || 
            token[i] == '\n' ||
            token[i] == '\r' ||
            token[i] == '\v') 
        {
            return false;
        }


        if (token[i] != '\\') {
            return true;
        }

        if (i == token.length()-1) {
            return !exact;
        }
        i++;
        if (token[i] == 'a'  ||
            token[i] == 'b'  ||
            token[i] == 'f'  ||
            token[i] == 'n'  ||
            token[i] == 'r'  ||
            token[i] == 't'  ||
            token[i] == 'v'  ||
            token[i] == '\\' ||
            token[i] == '\'' ||
            token[i] == '\"')
        {   
            return true;
        }
        else if (token[i] == '0' ||
                 token[i] == '1' ||
                 token[i] == '2' ||
                 token[i] == '3' ||
                 token[i] == 'x')
        {
            const auto& digits = (token[i] == 'x' ? hex : oct);
            if (i == token.length()-1) {
                return !exact;
            }
            i++;
            if (digits.find(token[i]) == std::string::npos) {
                return false;
            }
            if (i == token.length()-1) {
                return !exact;
            }
            i++;
            if (digits.find(token[i]) == std::string::npos) {
                return false;
            }
            return true;
        }
        return false;
    }

}
