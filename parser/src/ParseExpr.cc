#include <Parser.h>
#include <TokenID.h>
#include <TypeNode.h>
#include <LiteralNode.h>
#include <ExprNode.h>

namespace avl {

    bool Parser::parseLiteral(std::size_t it) {

        std::size_t n = 0;

        if (isLiteral(it)) {
            std::string err = "";
            if (parseToken(it, TOKEN_INT)) {
                result = IntNode::construct(tokens[it]);
                err = "Integer out of range";
            }
            else if (parseToken(it, TOKEN_REAL)) {
                result = RealNode::construct(tokens[it]);
                err = "Real out of range";
            }
            else if (parseToken(it, TOKEN_CHAR)) {
                result = CharNode::construct(tokens[it]);
                err = "Multi-byte characters not allowed";
            }
            else if (parseToken(it, TOKEN_TRUE) || parseToken(it, TOKEN_FALSE)) {
                result = BoolNode::construct(tokens[it]);
            }
            else {
                auto strnode = StringNode::construct(tokens[it]);
                while (parseToken(it+n+1, TOKEN_STRING)) {
                    auto next_strnode = StringNode::construct(tokens[it+n+1]);
                    strnode->str += next_strnode->str;
                    strnode->literal += next_strnode->literal;
                    strnode->loc.end = next_strnode->loc.end;
                    n++;
                }
                result = strnode;
            }
            if (!result) {
                return error(tokens[it], err);
            }
            n++;
        }

        if (n > 0) {
            return success(n);
        }
        return error();
    }

    bool Parser::parseUnary(std::size_t it) {

        std::size_t n = 0;
        auto loc = tokens[it]->loc;

        if (parseToken(it, TOKEN_ROUND_OPEN)) {
            n++;
            if (!parseExpr(it+n)) {
                return error(tokens[it+n], "Failed to parse expression after \'(\'");
            }
            n += nParsed;
            if (!parseToken(it+n, TOKEN_ROUND_CLOSE)) {
                return error(tokens[it+n], "Expecting \')\'");
            }
            loc.end = tokens[it+n]->loc.end;
            result->loc = loc;
            n++;
        }
        else if (parseLiteral(it)) {
            n += nParsed;
        }
        else if (parsePreOpUnary(it)) {
            n += nParsed;
        }
        else if (parseToken(it, TOKEN_IDENT)) {
            result = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
            n++;
            while (parseToken(it+n, TOKEN_DOT) ||
                   parseToken(it+n, TOKEN_DEREF) ||
                   parseToken(it+n, TOKEN_SQUARE_OPEN) ||
                   parseToken(it+n, TOKEN_ROUND_OPEN))
            {
                if (!parsePostOpUnary(it+n)) {
                    return error();
                }
                n += nParsed;
            }
        }

        if (n > 0 && parseToken(it+n, TOKEN_CAST)) {
            auto e = result;
            std::shared_ptr<Node> cast;
            n++;
            if (parseToken(it+n, TOKEN_IDENT)) {
                cast = std::make_shared<Identifier>(tokens[it+n]->str, tokens[it+n]->loc);
                n++;
            }
            else if (parseType(it+n)) {
                cast = result;
                n += nParsed;
            }
            else {
                return error(tokens[it+n], "Failed to parse recast type");
            }
            loc.end = cast->loc.end;
            result = std::make_shared<BinaryExprNode>(BINARYOP_RECAST, e, cast);
            result->loc = loc;
        }

        if (n > 0) {
            return success(n);
        }
        return error();

    }

    bool Parser::parsePreOpUnary(std::size_t it) {

        std::size_t n = 0;
        auto loc = tokens[it]->loc;

        if (isUnaryOp(it)) {
            auto op = ExprNode::unopFromToken(tokens[it]->is);
            bool issizeop = parseToken(it, TOKEN_SIZE);
            if ((issizeop && parseType(it+1)) || parseUnary(it+1)) {
                loc.end = result->loc.end;
                result = std::make_shared<UnaryExprNode>(op, result);
                result->loc = loc;
                n += 1+nParsed;
            }
            else {
                return error(tokens[it+1], "Failed to parse expression after " + tokens[it]->str);
            }
        }

        if (n > 0) {
            return success(n);
        }
        return error();
    }

    bool Parser::parsePostOpUnary(std::size_t it) {

        if (!result) {
            return error();
        }

        std::size_t n = 0;
        auto loc = result->loc;

        if (parseToken(it, TOKEN_DOT)) {
            n++;
            if (!parseToken(it+1, TOKEN_IDENT)) {
                return error(tokens[it+1], "Failed to parse member after \'.\'");
            }
            auto member = std::make_shared<Identifier>(tokens[it+1]->str, tokens[it+1]->loc);
            result = std::make_shared<BinaryExprNode>(BINARYOP_MEMBER, result, member);
        }
        else if (parseToken(it, TOKEN_DEREF)) {
            result = std::make_shared<UnaryExprNode>(UNARYOP_DEREFERENCE, result);
        }
        else if (parseToken(it, TOKEN_SQUARE_OPEN)) {
            auto e = result;
            if (!parseExpr(it+1)) {
                return error(tokens[it+1], "Failed to parse index element");
            }
            n += 1+nParsed;
            if (!parseToken(it+n, TOKEN_SQUARE_CLOSE)) {
                return error(tokens[it+n], "Missing \']\'");
            }
            result = std::make_shared<BinaryExprNode>(BINARYOP_ELEMENT, e, result);
        }
        else if (parseToken(it, TOKEN_ROUND_OPEN)) {
            auto e = result;
            std::vector<std::shared_ptr<Node> > argv;
            n++;
            while (true) {
                if (parseToken(it+n, TOKEN_ROUND_CLOSE)) {
                    break;
                }
                if (!parseExpr(it+n)) {
                    return error(tokens[it+n], "Failed to parse argument");
                }
                n += nParsed;
                argv.push_back(result);
        
                if (parseToken(it+n, TOKEN_COMMA)) {
                    if (parseToken(it+n+1, TOKEN_ROUND_CLOSE)) {
                        return error(tokens[it+n+1], "Cannot terminate argument list with a \',\'");
                    }
                    n++;
                }
            }
            result = std::make_shared<CallExprNode>(e, argv);
        }
        else {
            return error();
        }


        loc.end = tokens[it+n]->loc.end;
        result->loc = loc;
        n++;
        return success(n);
    }

    bool Parser::parseExpr(std::size_t it) {

        std::size_t n = 0;

        std::shared_ptr<Node> lhs;
        std::shared_ptr<Node> exp;
        if (!parseUnary(it)) {
            return error();
        }
        n += nParsed;
        lhs = result;
        if (!parseBinaryOperationRHS(it+n, 0, std::move(lhs))) {
            return error();
        }
        n += nParsed;
        lhs = result;

        if (isAssigner(it+n)) {
            int op = ExprNode::assopFromToken(tokens[it+n]->is);
            n++;
            if (!parseExpr(it+n)) {
                return error(tokens[it+n], "Failed to parse expression after " + tokens[it+n-1]->str);
            }
            n += nParsed;
            exp = result;
            result = std::make_shared<AssignExprNode>(op, lhs, exp);
            result->loc = lhs->loc;
            result->loc.end = exp->loc.end;
        }

        return success(n);
    }

    bool Parser::parseBinaryOperationRHS(std::size_t it, int prec, const std::shared_ptr<Node>& l) {

        std::size_t n = 0;
        std::shared_ptr<Node> lhs = l;
        auto loc = lhs->loc;

        while (true) {

            int current_op = tokens[it+n]->is;
            int current_prec = isBinaryMathOp(it+n) ? ExprNode::precedence(current_op) : -1;

            if (current_prec < prec) {
                result = lhs;
                return success(n);
            }
            n++;

            if (!parseUnary(it+n)) {
                return error(tokens[it+n], "Failed to parse expression");
            }
            n += nParsed;
            auto rhs = result;

            int next_op = tokens[it+n]->is;
            int next_prec = ExprNode::precedence(next_op);
            if (next_prec <= 0) {
                next_prec = -1;
            }

            if (next_prec > current_prec) {
                if (!parseBinaryOperationRHS(it+n, current_prec+1, std::move(rhs))) {
                    return error(tokens[it+n], "Failed to parse expression");
                }
                n += nParsed;
                rhs = result;
            }

            loc.end = rhs->loc.end;
            lhs = std::make_shared<BinaryExprNode>(ExprNode::binopFromToken(current_op), lhs, rhs);
            lhs->loc = loc;
        }
    }

}
