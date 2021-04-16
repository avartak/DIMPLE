#include <iostream>
#include <sstream>
#include <Parser.h>
#include <TokenID.h>
#include <Initializer.h>
#include <Statement.h>
#include <BlockNode.h>

namespace avl {

    /*

    FUNCTION_BLOCK : BLOCK

    */

    bool Parser::parseFunc(std::size_t it) {

        if (parseToken(it, TOKEN_CURLY_OPEN) && parseToken(it+1, TOKEN_CURLY_CLOSE)) {
            auto nullinit = std::make_shared<NullInit>(true);
            nullinit->loc = tokens[it]->loc;
            nullinit->loc.end = tokens[it+1]->loc.end;
            result = nullinit;
            return success(2);
        }

        std::shared_ptr<BlockNode> func = std::make_shared<FuncBlockNode>();
        if (parseBlock(it, func)) {
            result = func;
            return true;       
        }
        return error();
        
    }

    /*

    BLOCK : '{' ONE_OR_MORE_STATEMENTS '}'

    ONE_OR_MORE_STATEMENTS : STATEMENT_OR_BLOCK | STATEMENT_OR_BLOCK ONE_OR_MORE_STATEMENTS

    STATEMENT_OR_BLOCK : ';' | STATEMENT ';' | IF_BLOCK | LOOP_BLOCK

    */

    bool Parser::parseBlock(std::size_t it, const std::shared_ptr<BlockNode>& b) {

        std::size_t n = 0;

        if (!parseToken(it, TOKEN_CURLY_OPEN)) {
            return error();
        }
        b->loc = tokens[it]->loc;
        n++;
        std::size_t count = 0;
        while (true) {
            std::shared_ptr<Statement> stmt;
            auto loc = tokens[it+n]->loc;
            if (parseToken(it+n, TOKEN_CURLY_CLOSE)) {
                b->loc.end = tokens[it+n]->loc.end;
                n++;
                break;
            }
            else if (parseToken(it+n, TOKEN_SEMICOLON)) {
                n++;
            }
            else if (parseStatement(it+n, b)) {
                stmt = std::static_pointer_cast<Statement>(result);
                n += nParsed;
                if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                    return error(tokens[it+n], "Expect \';\' at the end of statement");
                }
                n++;
            }
            else if (parseIf(it+n, b) || parseLoop(it+n, b)) {
                stmt = std::static_pointer_cast<Statement>(result);
                n += nParsed;
            }
            else {
                if (count == 0) {
                    return error();
                }
                return error(tokens[it+n], "Failed to parse statement");
            }

            stmt->loc = loc;
            stmt->loc.end = tokens[it+n-1]->loc.end;
            b->body.push_back(stmt);
            count++;
        }

        return success(n);
    }

    /*

    STATEMENT : 'continue' | 'break' | 'return' | 'return' EXPR | LOCAL_VAR_DEF | LOCAL_REF_DEF | ASSIGN_STATEMENT | CALL_STATEMENT

    ASSIGN_STATEMENT : EXPR_ASSIGN

    CALL_STATEMENT : POSTOP_UNARY_CALL

    */

    bool Parser::parseStatement(std::size_t it, const std::shared_ptr<BlockNode>& b) {
        
        std::size_t n = 0;

        if (parseToken(it, TOKEN_CONTINUE)) {
            n++;
            result = std::make_shared<ContinueStatement>();
        }
        else if (parseToken(it, TOKEN_BREAK)) {
            n++;
            result = std::make_shared<BreakStatement>();
        }
        else if (parseToken(it, TOKEN_RETURN)) {
            n++;
            std::shared_ptr<Node> retval;
            if (parseExpr(it+n)) {
                n += nParsed;
                retval = result;
            }
            result = std::make_shared<ReturnStatement>(retval);
        }
        else if (parseLocalVarDef(it, b)) {
            n += nParsed;
        }
        else if (parseLocalRefDef(it, b)) {
            n += nParsed;
        }
        else if (parseExpr(it)) {
            n += nParsed;
            auto exp = static_cast<ExprNode*>(result.get());
            if (exp->is == EXPR_ASSIGN) {
                auto assign = std::static_pointer_cast<AssignExprNode>(result);
                result = std::make_shared<AssignStatement>(assign);
            }
            else if (exp->is == EXPR_CALL) {
                auto call = std::static_pointer_cast<CallExprNode>(result);
                result = std::make_shared<CallStatement>(call);
            }
            else {
                return error(result, "Statement expression not an assignment or function call");
            }
        }
        else {
            return error();
        }

        return success(n);

    }

    /*

    IF_BLOCK : ONE_OR_MORE_IFS | ONE_OR_MORE_IFS ELSE_BLOCK

    ONE_OR_MORE_IFS : 'if' EXPR BLOCK | 'if' EXPR BLOCK 'else' ONE_OR_MORE_IFS

    ELSE_BLOCK : 'else' BLOCK

    */

    bool Parser::parseIf(std::size_t it, const std::shared_ptr<BlockNode>& b) {
        
        std::size_t n = 0;

        bool if_done = false;
        auto ifblock = std::make_shared<IfBlockNode>(b);
        while (true) {
            auto loc = tokens[it+n]->loc;
            std::shared_ptr<Node> ifcond;
            if (parseToken(it+n, TOKEN_IF)) {
                n++;
                if (!parseExpr(it+n)) {
                    return error(tokens[it+n], "Failed to parse \'if\' condition expression");
                }
                n += nParsed;
                ifcond = result;
                if_done = true;
            }
            if (!if_done) {
                return error();
            }
            auto cond_block = std::make_shared<CondBlockNode>(ifblock);
            cond_block->condition = ifcond;
            if (!parseBlock(it+n, cond_block)) {
                return error(tokens[it+n], "Failed to parse \'if\' block");
            }
            cond_block->loc.start = loc.start;
            ifblock->body.push_back(cond_block);
            n += nParsed;
            if (!ifcond) {
                break;
            }
            if (!parseToken(it+n, TOKEN_ELSE)) {
                break;
            }
            n++;
        }

        result = ifblock;
        return success(n);
    }

    /*

    LOOP_BLOCK : 'loop' BLOCK | 
                 'loop' EXPR BLOCK | 
                 'loop' [EXPR] ';' [EXPR] ';' [EXPR] BLOCK

    */

    bool Parser::parseLoop(std::size_t it, const std::shared_ptr<BlockNode>& b) {
        
        std::size_t n = 0;
       
        auto loop = std::make_shared<LoopBlockNode>(b);
        auto cond_block = std::make_shared<CondBlockNode>(loop);
        std::shared_ptr<Statement> init;
        std::shared_ptr<Statement> update;

        if (!parseToken(it, TOKEN_LOOP)) {
            return error();
        }
        n++;

        if (parseExpr(it+n) && parseToken(it+n+nParsed, TOKEN_CURLY_OPEN)) {
            cond_block->condition = result;
            n += nParsed;
        }
        else if (parseToken(it+n, TOKEN_SEMICOLON) || parseStatement(it+n, loop)) {
            if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                init = std::static_pointer_cast<Statement>(result);
                init->loc = tokens[it+n]->loc;
                n += nParsed;
                init->loc.end = tokens[it+n-1]->loc.end;
                if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                    return error(tokens[it+n], "Expect \';\' at the end of the loop initialization statement");
                }
            }
            n++;

            if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                if (!parseExpr(it+n)) {
                    return error(tokens[it+n], "Failed to parse loop termination expression");
                }
                cond_block->condition = result;
                n += nParsed;
                if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                    return error(tokens[it+n], "Expect \';\' at the end of the loop termination expression");
                }
            }
            n++;

            if (parseStatement(it+n, loop)) {
                update = std::static_pointer_cast<Statement>(result);
                update->loc = tokens[it+n]->loc;
                n += nParsed;
                update->loc.end = tokens[it+n-1]->loc.end;
            }
        }

        if (!parseBlock(it+n, cond_block)) {
            return error(tokens[it+n], "Failed to parse the loop block");
        }
        n += nParsed;

        loop->body.push_back(init);
        loop->body.push_back(cond_block);
        loop->body.push_back(update);

        result = loop;
        return success(n);
    }

    /*

    LOCAL_VAR_DEF : TOKEN_IDENT ':=' EXPR |
                    TOKEN_IDENT ':=' (TOKEN_IDENT | TYPE) INITIALIZER

    */

    bool Parser::parseLocalVarDef(std::size_t it, const std::shared_ptr<BlockNode>& b) {
 
        std::size_t n = 0;

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DEFINE)) {
            return error();
        }
        const auto& nm = tokens[it]->str;
        if (!isAvailableLocally(it, b)) {
            return error();
        }
        name = std::make_shared<Identifier>(nm, tokens[it]->loc);
        n += 2;

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
                return error(tokens[it+n], "Unable to parse the definition of local variable " + nm);
            }

            if (!parseInit(it+n)) {
                return error(tokens[it+n], "Unable to parse initializer of local variable " + nm);
            }
        }

        n += nParsed;
        def = result;
        auto vdef = std::make_shared<DefineStatement>(STORAGE_LOCAL, name, type, def);
        b->symbols[nm] = vdef->name;

        result = vdef;
        return success(n);
    }

    /*

    LOCAL_REF_DEF : TOKEN_IDENT ':=' EXPR

    */

    bool Parser::parseLocalRefDef(std::size_t it, const std::shared_ptr<BlockNode>& b) {

        std::size_t n = 0;

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> def;

        if (!parseToken(it, TOKEN_ADDRESS)) {
            return error();
        }
        n++;
        if (!parseToken(it+n, TOKEN_IDENT) || !parseToken(it+n+1, TOKEN_DEFINE)) {
            if (!parseToken(it+n, TOKEN_IDENT)) {
                return error(tokens[it+n], "Expect identifier after \'@\'");
            }
            else {
                return error(tokens[it+n], "Expect \':=\' after " + tokens[it+n]->str);
            }
        }
        const auto& nm = tokens[it+n]->str;
        if (!isAvailableLocally(it+n, b)) {
            return error();
        }
        name = std::make_shared<Identifier>(nm, tokens[it+n]->loc);
        n += 2;

        if (!parseExpr(it+n)) {
            return error(tokens[it+n], "Unable to parse referee expression for " + nm);
        }
        n += nParsed;
        def = result;

        auto vdef = std::make_shared<DefineStatement>(STORAGE_REFERENCE, name, nullptr, def);
        b->symbols[nm] = vdef->name;

        result = vdef;
        return success(n);
    }

    bool Parser::isAvailableLocally(std::size_t it, const std::shared_ptr<BlockNode>& b) {

        const auto& nm = tokens[it]->str;
        if (b->symbols.find(nm) != b->symbols.end()) {
            std::stringstream err;
            err << "Redefinition of " << nm << ". " << "Previous occurence at ";
            err << b->symbols[nm]->loc.filename(input) << ":" << b->symbols[nm]->loc.start.line;
            return error(tokens[it], err.str());
        }

        return true;
    }
}
