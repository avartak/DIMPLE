#include <fstream>
#include <IO/Location.h>
#include <IO/InputFile.h>
#include <Parser/Parser.h>
#include <Lexer/TokenID.h>
  
namespace dmp {

    Parser::Parser(InputManager* in, AST* tree):
        Pass(in, tree, nullptr)
    {
    }

    void Parser::fail() {
        nParsed = 0;
        result.reset();
    }

    bool Parser::success(std::size_t n) {
        nParsed = n;
        return Pass::success();
    }

    bool Parser::run() {
        tokens.push_back(scan());
        bool status = parseProg(tokens.size()-1);
        tokens.pop_back();
        return status;
    }

    std::shared_ptr<Token> Parser::scan() {
        const auto& scanner = input->currentInputFile->scanner;
        auto type = scanner->lex();
        auto str = scanner->token_string;
        Coordinate start(scanner->start_line, scanner->start_column);
        Coordinate end(scanner->end_line, scanner->end_column);
        Location loc(input->currentInputFile->index, start, end);

        return std::make_shared<Token>(type, str, loc);
    }

    bool Parser::parseToken(std::size_t it, int t) {
        if (tokens[it]->is != t) {
            return false;
        }

        if (t != TOKEN_UNDEF && t != TOKEN_ERROR && t != TOKEN_EOF) {
            if (it == tokens.size() - 1) {
                tokens.push_back(scan());
            }
            return true;
        }

        if (hasErrors()) {
            return true;
        }

        switch (tokens[it]->is) {
            case TOKEN_UNDEF : errors.push_back(Error(tokens[it]->loc, "Invalid token " + tokens[it]->str)); break;
            case TOKEN_ERROR : errors.push_back(Error(tokens[it]->loc, "Unexpected termination of input"));
        }
        return true;
    }

    bool Parser::isUnaryOp(std::size_t it) {

        return (parseToken(it, TOKEN_PLUS)       || parseToken(it, TOKEN_MINUS) ||
                parseToken(it, TOKEN_COMPLEMENT) || parseToken(it, TOKEN_NOT)   ||
                parseToken(it, TOKEN_ADDRESS)    || parseToken(it, TOKEN_SIZE));

    }

    bool Parser::isBinaryMathOp(std::size_t it) {

        return (parseToken(it, TOKEN_LOG_OR)     || parseToken(it, TOKEN_LOG_AND)    ||
                parseToken(it, TOKEN_EQUAL)      || parseToken(it, TOKEN_NOT_EQUAL)  ||
                parseToken(it, TOKEN_GREATER)    || parseToken(it, TOKEN_LESSER)     ||
                parseToken(it, TOKEN_GEQ)        || parseToken(it, TOKEN_LEQ)        ||
                parseToken(it, TOKEN_BIT_OR)     || parseToken(it, TOKEN_BIT_XOR)    ||
                parseToken(it, TOKEN_BIT_AND)    || parseToken(it, TOKEN_BIT_LEFT)   ||
                parseToken(it, TOKEN_BIT_RIGHT)  || parseToken(it, TOKEN_PLUS)       ||
                parseToken(it, TOKEN_MINUS)      || parseToken(it, TOKEN_MULTIPLY)   ||
                parseToken(it, TOKEN_DIVIDE)     || parseToken(it, TOKEN_REMAINDER));

    }

    bool Parser::isAssigner(std::size_t it) {

        return (parseToken(it, TOKEN_ADD_ASSIGN) || parseToken(it, TOKEN_SUB_ASSIGN) ||
                parseToken(it, TOKEN_MUL_ASSIGN) || parseToken(it, TOKEN_DIV_ASSIGN) ||
                parseToken(it, TOKEN_REM_ASSIGN) || parseToken(it, TOKEN_AND_ASSIGN) ||
                parseToken(it, TOKEN_XOR_ASSIGN) || parseToken(it, TOKEN_OR_ASSIGN ) ||
                parseToken(it, TOKEN_BIT_RIGHT_ASSIGN) || parseToken(it, TOKEN_BIT_LEFT_ASSIGN)  || 
                parseToken(it, TOKEN_ASSIGN));
    }

    bool Parser::isLiteral(std::size_t it) {
        return (parseToken(it, TOKEN_TRUE) || parseToken(it, TOKEN_FALSE) ||
                parseToken(it, TOKEN_INT ) || parseToken(it, TOKEN_REAL ) ||
                parseToken(it, TOKEN_CHAR) || parseToken(it, TOKEN_STRING));
    }
}
