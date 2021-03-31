#include <sstream>
#include <Parser.h>
#include <TokenID.h>
#include <Initializer.h>
#include <Statement.h>
#include <BlockNode.h>

namespace avl {

    bool Parser::parseFunc(std::size_t it) {

        std::shared_ptr<BlockNode> func = std::make_shared<FuncBlockNode>();
        if (parseBlock(it, func)) {
            result = func;
            return true;       
        }
        return error();
        
    }

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

            if (!parseExpr(it+n)) {
                return error(tokens[it+n], "Failed to parse loop termination expression");
            }
            cond_block->condition = result;
            n += nParsed;
            if (!parseToken(it+n, TOKEN_SEMICOLON)) {
                return error(tokens[it+n], "Expect \';\' at the end of the loop termination expression");
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

    bool Parser::parseLocalVarDef(std::size_t it, const std::shared_ptr<BlockNode>& b) {
 
        std::size_t n = 0;

        std::shared_ptr<Identifier> name;
        std::shared_ptr<Node> type;
        std::shared_ptr<Node> def;

        if (!parseToken(it, TOKEN_IDENT) || !parseToken(it+1, TOKEN_DEFINE)) {
            return error();
        }
        name = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
        n += 2;

        if (parseToken(it+n, TOKEN_IDENT)) {
            type = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
        }
        else if (parseDataType(it+n)) {
            type = result;
            n += nParsed;
        }
        else {
            return error(tokens[it+n], "Failed to determine type of local variable " + tokens[it]->str);
        }

        if (!parseToken(it+n, TOKEN_CURLY_OPEN)) {
            def = std::make_shared<NullInit>(false);
        }
        else if (parseInit(it+n)) {
            n += nParsed;
            def = result;
        }
        else {
            return error(tokens[it+n], "Failed to parse initializer of local variable " + tokens[it]->str);
        }

        auto vdef = std::make_shared<DefineStatement>(STORAGE_LOCAL, name, type, def);
        auto vname = vdef->name->name;
        if (b->vars.find(vdef->name->name) != b->vars.end()) {
            std::stringstream err;
            err << "Redefinition of " << vname << ". " << "Previous occurence at ";
            err << b->vars[vname]->loc.filename(input.get()) << ":" << b->vars[vname]->loc.start.line;
            return error(vdef->name, err.str());
        }
        b->vars[vname] = vdef->name;

        result = vdef;
        return success(n);
    }

}
