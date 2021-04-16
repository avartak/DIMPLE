#include <sstream>
#include <Parser.h>
#include <TokenID.h>
#include <Globals.h>

namespace avl {

    /*

    PROG : TOKEN_EOF | UNIT PROG

    UNIT : EMPTY          | 
           INCLUDE        | 
           REPRESENTATION |
           DECLARATION    |
           DEFINITION     |
           REF_DEF

    */

    bool Parser::parseProg(std::size_t it) {

        if (parseToken(it, TOKEN_EOF)) {
            return success(0);
        }

        if (parseEmpty(it) ||
            parseInclude(it) ||
            parseRepresentation(it) ||
            parseDeclaration(it) ||
            parseDefinition(it) ||
            parseRefDef(it))
        {
            tokens.erase(tokens.begin()+it, tokens.begin()+it+nParsed);
            return parseProg(it);
        }

        if (hasErrors()) {
            return error();
        }
        return error(tokens[it], "Unable to parse unit");

    }

    /*

    EMPTY : ';'

    */

    bool Parser::parseEmpty(std::size_t it) {

        if (parseToken(it, TOKEN_SEMICOLON)) {
            return success(1);
        }
        return error();
    
    }

    /*

    INCLUDE : 'include' TOKEN_FILENAME '\n'

    */

    bool Parser::parseInclude(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_INCLUDE)) {
            return error();
        }
        n++;
        if (!parseToken(it+n, TOKEN_FILENAME)) {
            return error(tokens[it+n], "Expect a valid filename after \'include\'");
        }

        auto filename = tokens[it+n]->str;
        while (filename[0] == ' '  ||
               filename[0] == '\f' ||
               filename[0] == '\n' ||
               filename[0] == '\r' ||
               filename[0] == '\t' ||
               filename[0] == '\v')
        {
            filename.erase(filename.begin());
        }
        while (filename.back() == ' '  ||
               filename.back() == '\f' ||
               filename.back() == '\n' ||
               filename.back() == '\r' ||
               filename.back() == '\t' ||
               filename.back() == '\v')
        {
            filename.erase(filename.end()-1);
        }

        if (input->isActive(filename)) {
            return error(tokens[it+n], "Circular inclusion of " + filename);
        }
        if (input->isProcessed(filename)) {
            return success(n);
        }
        input->set(filename);
        if (!input->isValid()) {
            return error(tokens[it+n], "Unable to open file " + filename);
        }
        if (!run()) {
            return error();
        }
        input->reset();

        n++;
        return success(n);

    }

    /*

    REPRESENTATION : TOKEN_IDENT '::' (TYPE | EXPR) [';']

    */

    bool Parser::parseRepresentation(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_REPRESENTS)) {
            return error();
        }

        std::shared_ptr<Identifier> name;

        if (!isAvailable(it)) {
            return error();
        }
        const auto& nm = tokens[it]->str;
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 2;

        if (parseType(it+n) || parseExpr(it+n)) {
            ast->representations[nm] = std::make_shared<NameNode>(name, result);
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Unable to parse the definition of representation " + nm);
        }

        if (parseToken(it+n, TOKEN_SEMICOLON)) {
            n++;
        }
        return success(n);
    }

    /*

    DECLARATION : TOKEN_IDENT ':' (TOKEN_IDENT | TYPE) [';']

    */

    bool Parser::parseDeclaration(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DECLARE)) {
            return error();
        }

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;

        if (!isAvailable(it)) {
            return error();
        }
        const auto& nm = tokens[it]->str;
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 2;

        if (parseToken(it+n, TOKEN_IDENT)) {
            type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseType(it+n)) {
            type = result;
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Unable to parse declaration type");
        }

        ast->declarations[nm] = std::make_shared<NameNode>(name, type);

        if (parseToken(it+n, TOKEN_SEMICOLON)) {
            n++;
        }
        return success(n);
    }

    /*

    DEFINITION : ['extern'] TOKEN_IDENT ':=' (TOKEN_IDENT | TYPE) (INITIALIZER | FUNCTION_BLOCK) [';'] |
                 ['extern'] TOKEN_IDENT ':=' EXPR [';']

    */

    bool Parser::parseDefinition(std::size_t it) {

        std::size_t n = 0;

        uint16_t storage = STORAGE_INTERNAL;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;
        bool isMain = false;

        if (parseToken(it, TOKEN_EXTERN)) {
            storage = STORAGE_EXTERNAL;
            n++;
        }

        if (!parseToken(it+n, TOKEN_IDENT) && !parseToken(it+n, TOKEN_MAIN)) {
            return (storage == STORAGE_EXTERNAL ? error(tokens[it+n], "Expect global name after \'extern\'") : error());
        }
        if (parseToken(it+n, TOKEN_MAIN)) {
            isMain = true;
        }
        const auto& nm = tokens[it+n]->str;
        name = std::make_shared<Identifier>(nm, tokens[it+n]->loc);
        n++;

        if (!parseToken(it+n, TOKEN_DEFINE)) {
            return (storage == STORAGE_INTERNAL ? error() : error(tokens[it+n], "Expect \':=\' after " + nm + " in global definition"));
        }
        if (!isAvailable(it+n-1)) {
            return error();
        }
        n++;

        if (!parseExpr(it+n) || parseToken(it+n+nParsed, TOKEN_CURLY_OPEN)) {
            if (parseToken(it+n, TOKEN_IDENT)) {
                type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                n++;
            }
            else if (parseType(it+n)) {
                type = result;
                n += nParsed;
            }
            else {
                return error(tokens[it+n], "Unable to parse the definition of " + nm);
            }

            if (parseInit(it+n)) {
                if (isMain) {
                    return error(tokens[it+n], "\'main\' can only be a function");
                }
            }
            else if (parseFunc(it+n)) {
            }
            else {
                return error(tokens[it+n], "Unable to parse the definition of " + nm);
            }
        }

        n += nParsed;
        def = result;
        ast->definitions[nm] = std::make_shared<DefineStatement>(storage, name, type, def);

        if (parseToken(it+n, TOKEN_SEMICOLON)) {
            n++;
        }
        return success(n);
    }

    /*

    REF_DEF : '@' TOKEN_IDENT ':=' EXPR [';']

    */

    bool Parser::parseRefDef(std::size_t it) {

        std::size_t n = 0;

        uint16_t storage = STORAGE_REFERENCE;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        if (!parseToken(it, TOKEN_ADDRESS)) {
            return error();
        }
        n++;

        if (!parseToken(it+n, TOKEN_IDENT)) {
            return error(tokens[it+n], "Expect identifier after \'@\'");
        }
        const auto& nm = tokens[it+n]->str;
        name = std::make_shared<Identifier>(nm, tokens[it+n]->loc);
        n++;

        if (!parseToken(it+n, TOKEN_DEFINE)) {
            return (storage == STORAGE_INTERNAL ? error() : error(tokens[it+n], "Expect \':=\' after " + nm + " in global definition"));
        }
        if (!isAvailable(it+n-1)) {
            return error();
        }
        n++;

        if (!parseExpr(it+n)) {
            return error(tokens[it+n], "Unable to parse the referee of global reference " + nm);
        }
        n += nParsed;
        def = result;
        ast->definitions[nm] = std::make_shared<DefineStatement>(storage, name, type, def);

        if (parseToken(it+n, TOKEN_SEMICOLON)) {
            n++;
        }
        return success(n);
    }

    bool Parser::isAvailable(std::size_t it) {

        std::shared_ptr<Identifier> prev;
        const auto& nm = tokens[it]->str;
        if (ast->representations.find(nm) != ast->representations.end()) {
            prev = ast->representations[nm]->name;
        }
        else if (ast->declarations.find(nm) != ast->declarations.end()) {
            prev = ast->declarations[nm]->name;
        }
        else if (ast->definitions.find(nm) != ast->definitions.end()) {
            prev = ast->definitions[nm]->name;
        }
        if (prev) {
            std::stringstream err;
            err << "Redefinition of " << nm << ". ";
            err << "Previous occurence at " << prev->loc.filename(input) << ":" << prev->loc.start.line;
            return error(tokens[it], err.str());
        }
        return true;
    }
} 
