#include <Parser.h>
#include <TokenID.h>
#include <Initializer.h>
  
namespace avl {

    bool Parser::parseUntaggedInitSet(std::size_t it) {

        std::size_t n = 0;

        std::vector<InitElement> iev;
        while (true) {
            if (!parseExpr(it+n) && !parseInit(it+n)) {
                if (iev.size() == 0) {
                    return error();
                }
                return error(tokens[it+n], "Failed to parse untagged rvalue in initializer set");
            }
            n += nParsed;
            iev.push_back(InitElement(result));
            if (!parseToken(it+n, TOKEN_COMMA)) {
                break;
            }
            n++;
        }
        result = std::make_shared<Initializer>(iev);
        return success(n);
    }

    bool Parser::parseTaggedInitSet(std::size_t it) {

        std::size_t n = 0;

        std::vector<InitElement> iev;
        while (true) {
            std::shared_ptr<Node> tag;
            std::shared_ptr<Node> rv;
            bool indexed = false;
            auto loc = tokens[it+n]->loc;

            if (parseToken(it+n, TOKEN_SQUARE_OPEN)) {
                n++;
                if (!parseExpr(it+n)) {
                    return error(tokens[it+n], "Failed to parse index of rvalue in initializer set");
                }
                n += nParsed;
                tag = result;
                if (!parseToken(it+n, TOKEN_SQUARE_OPEN)) {
                    return error(tokens[it+n], "Expect \']\' to terminate the index in initializer");
                }
                n++;
                indexed = true;
            }
            else if (parseToken(it+n, TOKEN_DOT)) {
                n++;
                if (!parseToken(it+n, TOKEN_IDENT)) {
                    return error(tokens[it+n], "Failed to parse member tag in initializer set");
                }
                tag = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                n++;
            }
            else {
                if (iev.size() == 0) {
                    return error();
                }
                return error(tokens[it+n], "Failed to parse the tag in initializer set");
            }

            if (!parseToken(it+n, TOKEN_ASSIGN)) {
                return error(tokens[it+n], "Expect \'=\' after rvalue tag in initializer set");
            }
            n++;

            if (!parseExpr(it+n) && !parseInit(it+n)) {
                return error(tokens[it+n], "Failed to parse tagged rvalue in initializer set");
            }
            n += nParsed;
            rv = result;

            auto tagtype = (indexed ? INIT_INDEXED : INIT_LABELED);
            InitElement ie(tagtype, tag, rv);
            ie.loc.start = loc.start;
            iev.push_back(ie);

            if (!parseToken(it+n, TOKEN_COMMA)) {
                break;
            }
            n++;
        }

        result = std::make_shared<Initializer>(iev);
        return success(n);
    }

    bool Parser::parseInit(std::size_t it) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_CURLY_OPEN)) {
            return error();
        }
        n++;

        if (parseToken(it+n, TOKEN_CURLY_CLOSE)) {
            result = std::make_shared<NullInit>(true);
        }
        else if (parseUntaggedInitSet(it+n) || 
                 parseTaggedInitSet(it+n)) 
        {
            n += nParsed;
        } 
        else return error();

        if (!parseToken(it+n, TOKEN_CURLY_CLOSE)) {
            return error(tokens[it+n], "Expect \'}\' at the end of initializer set");
        }
        n++;

        auto loc = tokens[it]->loc;
        loc.end = tokens[it+n-1]->loc.end;
        result->loc = loc;
        return success(n);
    }


}

