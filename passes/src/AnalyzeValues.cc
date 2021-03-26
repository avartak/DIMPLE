#include <Analyzer.h>
#include <Literal.h>
#include <LiteralNode.h>
#include <UnaryOp.h>
#include <BinaryOp.h>

namespace avl {

    bool Analyzer::getValue(const std::shared_ptr<Node>& node) {
        if (node->kind == NODE_IDENTIFIER) {
            auto ident = std::static_pointer_cast<Identifier>(node);
            auto n = ident->name;
            if (currentFunction && currentFunction->scope->isDefined(n)) {
                result = currentFunction->scope->getVar(n);
                return success();
            }
            else if (variables.find(n) != variables.end()) {
                result = variables[n];
                return success();
            }
            else if (functions.find(n) != functions.end()) {
                result = functions[n];
                return success();
            }
            else if (ast->declarations.find(n) != ast->declarations.end() ||
                     ast->definitions.find(n) != ast->decfinitions.end()) 
            {
                if (getGlobalVar(ident)) {
                    return success();
                }
                if (hasErrors()) {
                    return error(ident, "Unable to create global instance " + n);
                }
                return getFunction(ident);
            }
            else {
                return error(ident, "Unable to decipher idenitifer " + n);
            }
        }

        if (node->kind != NODE_EXPRNODE) {
            return error();
        }

        auto expr = static_cast<ExprNode*>(node.get());
        if (expr->is == EXPR_INT  ||
            expr->is == EXPR_BOOL ||
            expr->is == EXPR_REAL ||
            expr->is == EXPR_CHAR ||
            expr->is == EXPR_STRING) 
        {
            return getLiteral(std::static_pointer_cast<ExprNode>(node));
        }
        else if (expr->is == EXPR_ASSIGN) {
            return getAssignExpr(std::static_pointer_cast<AssignExprNode>(node));
        }
        else if (expr->is == EXPR_CALL) {
            return getCallExpr(std::static_pointer_cast<CallExprNode>(node));
        }
        else if (expr->is == EXPR_UNARY) {
            return getUnaryExpr(std::static_pointer_cast<UnaryExprNode>(node));
        }
        else if (expr->is == EXPR_BINARY) {
            return getBinaryExpr(std::static_pointer_cast<BinaryExprNode>(node));
        }

        return error();

    }

        if (expr->is == EXPR_CALL) {
            auto callex = static_cast<CallExprNode*>(expr);
            if (!getValue(callex->func)) {
                return error(callex->func, "Unable to construct the function of the call expression");
            }
            auto fval = static_cast<Value*>(result.get());
            if (fval->is != VALUE_FUNC) {
                return error(callex->func, "A function call is placed on an instance/value that is not a function");
            }
            auto func = std::static_pointer_cast<Function>(result);
            if (!call(func, callex->args)) {
                return error(callex, "Function call failed");
            }
            // Function call result should already be stored in result
            return success();
        }

        else if (expr->is == EXPR_UNARY) {
            auto unary = static_cast<UnaryExprNode*>(expr);
            if (unary->op == UNARYOP_ADDRESS) {
                if (!createValue(unary->exp)) {
                    return error(unary, "Unable to evaluate the operand of the address operation");
                }
                auto val = std::static_pointer_cast<Value>(result);
                if (!val->isInstance() && !(val->type->isInt() && val->isConst())) {
                    return error(unary, "Operand of the address operation is neither an instance nor a constant integer");
                }
                const auto& addr = UnaryOp::address(val);
                if (!addr) {
                    return error(unary, "Address operation failed");
                }
                result = addr;
                return success();
            }
            else if (unary->op == UNARYOP_SIZE) {
                bool istype = false;
                if (!createType(unary->exp)) {
                    if (hasErrors()) {
                        return error();
                    }
                    if (!createValue(unary->exp)) {
                        return error(unary, "Unable to evaluate the operand of the size operation");
                    }
                }
                else {
                    istype = true;
                }
                if (istype) {
                    auto ty = std::static_pointer_cast<Type>(result);
                    if (!ty->isComplete()) {
                        return error(unary, "Attempting to size an incomplete type");
                    }
                    if (ty->isUnknown()) {
                        return error(unary, "<Internal> Attempting to size unknown type");
                    }
                    if (ty->isVoid()) {
                        return error(unary, "<Internal> Attempting to size void type");
                    }
                    const auto& sizety = UnaryOp::size(ty);
                    if (!sizety) {
                        return error(unary, "Unable to determine size of type");
                    }
                    result = sizety;
                    return success();
                }
                else {
                    auto v = std::static_pointer_cast<Value>(result);
                    if (!v->isVar()) {
                        return error(unary, "Operand of the size operation is not a variable");
                    }
                    const auto& sizev = UnaryOp::size(std::static_pointer_cast<Variable>(v));
                    if (!sizev) {
                        return error(unary, "Unable to determine size of variable");
                    }
                    result = sizev;
                    return success();
                }
            }
            else if (unary->op == UNARYOP_PLUS ||
                     unary->op == UNARYOP_NEGATE ||
                     unary->op == UNARYOP_COMPLEMENT ||
                     unary->op == UNARYOP_NOT ||
                     unary->op == UNARYOP_DEREFERENCE)
            {
                if (!createValue(unary->exp)) {
                    return error(unary, "Unable to evaluate the operand of the unary operation");
                }
                auto val = std::static_pointer_cast<Value>(result);
                std::shared_ptr<Value> res;
                if (unary->op == UNARYOP_PLUS) {
                    res = UnaryOp::plus(val);
                    if (!res) {
                        return error(unary, "Operand of \'+\' operator is non-numerical");
                    }
                }
                else if (unary->op == UNARYOP_NEGATE) {
                    res = UnaryOp::negate(val);
                    if (!res) {
                        return error(unary, "Operand of \'-\' operator is not a non-numerical");
                    }
                }
                else if (unary->op == UNARYOP_COMPLEMENT) {
                    res = UnaryOp::complement(val);
                    if (!res) {
                        return error(unary, "Operand of \'~\' operator is not an integer");
                    }
                }
                else if (unary->op == UNARYOP_NOT) {
                    res = UnaryOp::logicalNot(val);
                    if (!res) {
                        return error(unary, "Operand of \'!\' operator is not a boolean");
                    }
                }
                else if (unary->op == UNARYOP_DEREFERENCE) {
                    if (!val->type->isPtr()) {
                        return error(unary, "Attempting to dereference a non-pointer");
                    }
                    res = UnaryOp::dereference(val);
                    if (!res) {
                        return error(unary, "Dereference operation failed");
                    }
                }   
                if (res) {
                    result = res;
                    return success();
                }
            }
        }

        else if (expr->is == EXPR_BINARY) {
            auto binary = static_cast<BinaryExprNode*>(expr);
            if (binary->op == BINARYOP_RECAST) {
                if (!createType(binary->rhs)) {
                    return error(binary, "Unable to obtain recast type");
                }
                auto ty = std::static_pointer_cast<Type>(result);
                if (!createValue(binary->lhs)) {
                    return error(binary, "Unable to obtain expression to recast");
                }
                auto ex = std::static_pointer_cast<Value>(result);
                auto rex = BinaryOp::recast(ex, ty);
                if (!rex) {
                    return error(binary, "Recast failed");
                }
                result = rex;
                return success();
            }

            std::shared_ptr<Value> res;
            if (!createValue(binary->lhs)) {
                return error(binary, "Unable to evaluate the left side of the binary operation");
            }
            auto lhs = std::static_pointer_cast<Value>(result);

            if (binary->op == BINARYOP_MEMBER) {
                if (!lhs->isVar()) {
                    return error(binary, "Need a variable to perform the member operation");
                }
                if (!lhs->type->isStruct() && !lhs->type->isUnion()) {
                    return error(binary, "Variable type for the member operation must be struct or union");
                }
                // binary->rhs is guranteed to be a Identifier from parseExpr
                auto memident = static_cast<Identifier*>(binary->rhs.get());
                auto mem = std::make_shared<StringLiteral>(memident->name);
                auto var = std::static_pointer_cast<Variable>(lhs);
                res = BinaryOp::member(var, mem);
                if (!res) {
                    return error(binary, "Member operation failed");
                }
                result = res;
                return success();
            }

            if (!createValue(binary->rhs)) {
                return error(binary, "Unable to evaluate the right side of the binary operation");
            }
            auto rhs = std::static_pointer_cast<Value>(result);

            if (binary->op == BINARYOP_ELEMENT) {
                if (lhs->is != VALUE_VAR) {
                    return error(binary, "Need a variable to perform the element operation");
                }
                if (lhs->type->isPrimitive()) {
                    return error(binary, "Attempting to perform element operation on a primitive type");
                }
                if (!rhs->type->isInt()) {
                    return error(binary, "Element operand is not an integer");
                }
                auto var = std::static_pointer_cast<Variable>(lhs);
                res = BinaryOp::element(var, rhs);
                if (!res) {
                    return error(binary, "Element operation failed");
                }
            }

            else if (binary->op == BINARYOP_STR_CONCAT) {
                // Operands should ALWAYS be StringLiterals; no checks needed 
                auto lstr = std::static_pointer_cast<StringLiteral>(lhs);
                auto rstr = std::static_pointer_cast<StringLiteral>(rhs);
                res = BinaryOp::concat(lstr, rstr);
            }
            else if (binary->op == BINARYOP_ADD) {
                res = BinaryOp::add(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'+\' operation");
                }
            }
            else if (binary->op == BINARYOP_SUBTRACT) {
                res = BinaryOp::subtract(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'-\' operation");
                }
            }
            else if (binary->op == BINARYOP_MULTIPLY) {
                res = BinaryOp::multiply(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'*\' operation");
                }
            }
            else if (binary->op == BINARYOP_DIVIDE) {
                res = BinaryOp::divide(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'/\' operation");
                }
            }
            else if (binary->op == BINARYOP_MOD) {
                res = BinaryOp::remainder(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'%\' operation");
                }
            }
            else if (binary->op == BINARYOP_SHIFT_LEFT) {
                res = BinaryOp::shiftLeft(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'<<\' operation");
                }
            }
            else if (binary->op == BINARYOP_SHIFT_RIGHT) {
                res = BinaryOp::shiftRight(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'>>\' operation");
                }
            }
            else if (binary->op == BINARYOP_BIT_OR) {
                res = BinaryOp::bitOr(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'|\' operation");
                }
            }
            else if (binary->op == BINARYOP_BIT_AND) {
                res = BinaryOp::bitAnd(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'&\' operation");
                }
            }
            else if (binary->op == BINARYOP_BIT_XOR) {
                res = BinaryOp::bitXor(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'^\' operation");
                }
            }
            else if (binary->op == BINARYOP_LOGICAL_OR) {
                res = BinaryOp::logOr(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'||\' operation");
                }
            }
            else if (binary->op == BINARYOP_LOGICAL_AND) {
                res = BinaryOp::logAnd(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'&&\' operation");
                }
            }
            else if (binary->op == BINARYOP_EQUAL) {
                res = BinaryOp::equal(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'==\' operation");
                }
            }
            else if (binary->op == BINARYOP_NOT_EQUAL) {
                res = BinaryOp::unequal(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'!=\' operation");
                }
            }
            else if (binary->op == BINARYOP_GREATER) {
                res = BinaryOp::greater(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'>\' operation");
                }
            }
            else if (binary->op == BINARYOP_LESSER) {
                res = BinaryOp::lesser(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'<\' operation");
                }
            }
            else if (binary->op == BINARYOP_GREATER_EQUAL) {
                res = BinaryOp::greaterEqual(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'<=\' operation");
                }
            }
            else if (binary->op == BINARYOP_LESSER_EQUAL) {
                res = BinaryOp::lesserEqual(lhs, rhs);
                if (!res) {
                    return error(binary, "Unable to perform \'&&\' operation");
                }
            }

            if (!res) {
                return error();
            }
            result = res;
            return success();
        }

        return error();
    }

    bool Analyzer::getLiteral(const std::shared_ptr<ExprNode>& expr) {
        if (expr->is == EXPR_INT) {
            auto intnode = static_cast<IntNode*>(expr);
            result = std::make_shared<IntLiteral>(intnode->literal);
            return success();
        }
        else if (expr->is == EXPR_BOOL) {
            auto boolnode = static_cast<BoolNode*>(expr);
            result = std::make_shared<BoolLiteral>(boolnode->literal);
            return success();
        }
        else if (expr->is == EXPR_REAL) {
            auto realnode = static_cast<RealNode*>(expr);
            result = std::make_shared<RealLiteral>(realnode->literal);
            return success();
        }
        else if (expr->is == EXPR_CHAR) {
            auto charnode = static_cast<CharNode*>(expr);
            result = std::make_shared<CharLiteral>(charnode->literal);
            return success();
        }
        else if (expr->is == EXPR_STRING) {
            auto strnode = static_cast<StringNode*>(expr);
            result = std::make_shared<StringLiteral>(strnode->literal);
            return success();
        }
        else if (expr->is == EXPR_ASSIGN) {
            return error();
        }
    }

    bool Analyzer::getAssignExpr(const std::shared_ptr<AssignExprNode>& assex) {

        auto assex = static_cast<AssignExprNode*>(expr);
        if (!getValue(assex->lhs)) {
            return hasErrors() ? error() : error(assex, "Unable to construct left side of the assign expression");
        }
        auto lval = static_cast<Value*>(result.get());
        if (lval->is != VALUE_VAR) {
            return error(assex, "Left side of the assign expression is not instantiable");
        }
        auto lvar = std::static_pointer_cast<Variable>(result);
        return assign(assex->op, lvar, assex->rhs) {
    }
}
