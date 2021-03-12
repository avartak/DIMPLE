#include <sstream>
#include <Parser.h>
#include <TokenID.h>
#include <Globals.h>

namespace avl {

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

    bool Parser::parseEmpty(std::size_t it) {

        if (parseToken(it, TOKEN_SEMICOLON)) {
            return success(1);
        }
        return error();
    
    }

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

        if (TheInput->isActive(filename)) {
            return error(tokens[it+n], "Circular inclusion of " + filename);
        }
        if (TheInput->isProcessed(filename)) {
            return success(n);
        }
        TheInput->set(filename);
        if (!TheInput->isValid()) {
            return error(tokens[it+n], "Unable to open file " + filename);
        }
        if (!run()) {
            return error();
        }
        TheInput->reset();

        n++;
        return success(n);

    }

    bool Parser::parseRepresentation(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_REPRESENTS)) {
            return error();
        }

        std::shared_ptr<Identifier> name;

        auto nm = tokens[it]->str;
        if (ast->representations.find(nm) != ast->representations.end()) {
            name = ast->representations[nm]->name;
        }
        else if (ast->declarations.find(nm) != ast->declarations.end()) {
            name = ast->declarations[nm][0]->name;
        }
        else if (ast->definitions.find(nm) != ast->definitions.end()) {
            name = ast->definitions[nm]->name;
        }
        if (name) {
            std::stringstream err;
            err << "Invalid reassignment of " << nm << ". ";
            err << "Previous occurence at " << name->loc.filename() << ":" << name->loc.start.line;
            return error(tokens[it], err.str());
        }
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 2;

        if (parseType(it+n) || parseExpr(it+n)) {
            ast->representations[name->name] = std::make_shared<NameNode>(name, result);
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Expect representation definition");
        }

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    bool Parser::parseDeclaration(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DECLARE)) {
            return error();
        }

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;

        auto nm = tokens[it]->str;
        if (ast->representations.find(nm) != ast->representations.end()) {
            name = ast->representations[nm]->name;
        }
        else if (ast->definitions.find(nm) != ast->definitions.end()) {
            if (ast->definitions[nm]->storage == STORAGE_INTERNAL) {
                name = ast->representations[nm]->name;
            }
        }
        if (name) {
            std::stringstream err;
            err << "Invalid reassignment of " << nm << ". ";
            err << "Previous occurence at " << name->loc.filename() << ":" << name->loc.start.line;
            return error(tokens[it], err.str());
        }
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 2;

        if (parseToken(it+n, TOKEN_IDENT)) {
            type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseType(it+n) > 0) {
            type = result;
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Unable to parse the declaration of " + tokens[it+n-2]->str);
        }

        ast->declarations[name->name].push_back(std::make_shared<NameNode>(name, type));

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

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

        auto nm = tokens[it]->str;
        if (ast->representations.find(nm) != ast->representations.end()) {
            name = ast->representations[nm]->name;
        }
        else if (ast->declarations.find(nm) != ast->declarations.end() && storage == STORAGE_INTERNAL) {   
            name = ast->declarations[nm][0]->name;
        }
        else if (ast->definitions.find(nm) != ast->definitions.end()) {
            name = ast->definitions[nm]->name;
        }
        if (name) {
            std::stringstream err;
            err << "Invalid reassignment of " << nm << ". ";
            err << "Previous occurence at " << name->loc.filename() << ":" << name->loc.start.line;
            return error(tokens[it], err.str());
        }
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
            return error(tokens[it], "Unable to parse the definition of " + tokens[it]->str);
        }

        ast->definitions[name->name] = std::make_shared<Definition>(storage, name, type, def);

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }

    bool Parser::parseMain(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_MAIN)) {
            return error();
        }
        if (parseToken(it+1, TOKEN_DECLARE)) {
            return error(tokens[it], "Declaration of \'main\' is not allowed");
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

        auto nm = tokens[it]->str;
        if (ast->definitions.find(nm) != ast->definitions.end()) {
            name = ast->definitions[nm]->name;
        }
        if (name) {
            std::stringstream err;
            err << "Redefinition of " << nm << ". ";
            err << "Previous occurence at " << name->loc.filename() << ":" << name->loc.start.line;
            return error(tokens[it], err.str());
        }
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
            return error(tokens[it], "Unable to parse the definition of " + tokens[it]->str);
        }

        ast->definitions[name->name] = std::make_shared<Definition>(STORAGE_EXTERNAL, name, type, def);

        if (!parseTerm(it+n, false)) {
            return error();
        }
        n += nParsed;
        return success(n);
    }
} 
