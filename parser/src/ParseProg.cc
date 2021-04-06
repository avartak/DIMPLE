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
           MAIN_FUNCTION

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
            parseMain(it))
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

    REPRESENTATION : TOKEN_IDENT '::' (TYPE | EXPR) (';' | '\n' | TOKEN_EOF)

    */

    bool Parser::parseRepresentation(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_REPRESENTS)) {
            return error();
        }

        std::shared_ptr<Identifier> name;

        if (!isDefined(it)) {
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

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    /*

    DECLARATION : TOKEN_IDENT ':' (TOKEN_IDENT | TYPE) (';' | '\n' | TOKEN_EOF)

    */

    bool Parser::parseDeclaration(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DECLARE)) {
            return error();
        }

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;

        if (!isDefined(it)) {
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

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    /*

    DEFINITION : TOKEN_IDENT ':=' ['extern'] (TOKEN_IDENT | TYPE) (INITIALIZER | FUNCTION_BLOCK) (';' | '\n' | TOKEN_EOF)

    */

    bool Parser::parseDefinition(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DEFINE)) {
            return error();
        }

        uint16_t storage = STORAGE_INTERNAL;
        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        if (parseToken(it+2, TOKEN_EXTERN)) {
            storage = STORAGE_EXTERNAL;
        }

        if (!isDefined(it)) {
            return error();
        }
        const auto& nm = tokens[it]->str;
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += (storage == STORAGE_INTERNAL ? 2 : 3);

        if (parseToken(it+n, TOKEN_IDENT)) {
            type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseType(it+n)) {
            type = result;
            n += nParsed;
        }

        if (type && (parseInit(it+n) || parseFunc(it+n))) {
            n += nParsed;
            def = result;
        }

        if (!def) {
            return error(tokens[it], "Unable to parse the definition of " + nm);
        }

        ast->definitions[nm] = std::make_shared<DefineStatement>(storage, name, type, def);

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    /*

    DEFINITION : TOKEN_MAIN ':=' 'extern' (TOKEN_IDENT | TYPE) FUNCTION_BLOCK (';' | '\n' | TOKEN_EOF)

    */

    bool Parser::parseMain(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_MAIN)) {
            return error();
        }
        if (!parseToken(it+1, TOKEN_DEFINE)) {
            return error(tokens[it+1], "Expect \':=\' after \'main\'");
        }
        if (!parseToken(it+2, TOKEN_EXTERN)) {
            return error(tokens[it+2], "Expect \'extern\' specifier for \'main\' after \':=\'");
        }

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        if (!isDefined(it)) {
            return error();
        }
        const auto& nm = tokens[it]->str;
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 3;

        if (parseToken(it+n, TOKEN_IDENT)) {
            type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseType(it+n)) {
            type = result;
            n += nParsed;
        }

        if (type && parseFunc(it+n)) {
            n += nParsed;
            def = result;
        }

        if (!def) {
            return error(tokens[it], "Unable to parse the definition of " + nm);
        }

        ast->definitions[nm] = std::make_shared<DefineStatement>(STORAGE_EXTERNAL, name, type, def);

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    bool Parser::isDefined(std::size_t it) {

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
            err << "Previous occurence at " << prev->loc.filename(input.get()) << ":" << prev->loc.start.line;
            return error(tokens[it], err.str());
        }
        return true;
    }
} 
