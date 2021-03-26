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
            if (constants.find(n) != constants.end()) {
                if (!constants.find(n)->second) {
                    return(ident, "Cannot create a complete representation of " + n);
                }
                result = constants[n];
                return success();
            }
            else if (ast->representations.find(n) != ast->representations.end()) {
                auto next_node = ast->representations[n]->node;
                while (next_node->kind == NODE_IDENTIFIER) {
                    auto next_ident = std::static_pointer_cast<Identifier>(next_node);
                    const auto& nm = next_ident->name;
                    if (ast->representations.find(nm) == ast->representations.end()) {
                        return error(next_ident, "Identifier " + nm + " is not a representation");
                    }
                    next_node = ast->representations[nm]->node;                
                }
                if (next_node->kind != NODE_EXPRNODE) {
                    return error();
                }
                constants[n] = std::shared_ptr<Value>();
                if (!getValue(ast->representations[n]->node)) {
                    return error();
                }
                constants[n] = result;
                return success();
            }
            else if (currentFunction && currentFunction->scope->isDefined(n)) {
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

    bool Analyzer::getCallExpr(const std::shared_ptr<CallExprNode>& callex) {
        if (!getValue(callex->func)) {
            return hasErrors() ? error() : error(callex->func, "Unable to obtain function for the call expression");
        }
        auto fval = static_cast<Value*>(result.get());
        if (fval->is != VALUE_FUNC) {
            return error(callex->func, "A function call is placed on an instance/value that is not a function");
        }
        auto func = std::static_pointer_cast<Function>(result);
        return call(func, callex->args);
    }

    bool Analyzer::getUnaryExpr(const std::shared_ptr<UnaryExprNode>& unary) {
        if (unary->op == UNARYOP_ADDRESS) {
            if (!getValue(unary->exp)) {
                return error(unary, "Unable to evaluate the operand of the address operation");
            }
            auto val = std::static_pointer_cast<Value>(result);
            if (!val->isInstance() && !(val->type->isInt() && val->isConst())) {
                return error(unary, "Operand of address operation is neither an instance nor a constant integer");
            }
            result = UnaryOp::address(val);
            return success();
        }
        else if (unary->op == UNARYOP_SIZE) {
            bool istype = false;
            if (!getValue(unary->exp)) {
                if (hasErrors()) {
                    return error();
                }
                if (!getType(unary->exp, false)) {
                    return error(unary, "Unable to evaluate the operand of the size operation");
                }
                istype = true;
            }
            if (istype) {
                auto ty = std::static_pointer_cast<Type>(result);
                if (!ty->isComplete()) {
                    return error(unary, "Attempting to size an incomplete type");
                }
                if (ty->isUnknown()) {
                    return error(unary, "Attempting to size unknown type");
                }
                if (ty->isVoid()) {
                    return error(unary, "Attempting to size void type");
                }
                if (ty->isFunction()) {
                    return error(unary, "Attempting to size function type");
                }
                result = UnaryOp::size(ty); // result should always be valid
                return success();
            }
            else {
                auto v = std::static_pointer_cast<Value>(result);
                if (!v->isVar()) {
                    return error(unary, "Invalid operand of the size operation");
                }
                result = UnaryOp::size(std::static_pointer_cast<Variable>(v)); // result should always be valid
                return success();
            }
        }
        else if (unary->op == UNARYOP_PLUS ||
                 unary->op == UNARYOP_NEGATE ||
                 unary->op == UNARYOP_COMPLEMENT ||
                 unary->op == UNARYOP_NOT ||
                 unary->op == UNARYOP_DEREFERENCE)
        {
            if (!getValue(unary->exp)) {
                return error(unary, "Unable to evaluate the operand of unary operation");
            }
            auto val = std::static_pointer_cast<Value>(result);
            if (unary->op == UNARYOP_PLUS) {
                result = UnaryOp::plus(val);
                if (!result) {
                    return error(unary, "Operand of \'+\' operator is non-numerical");
                }
            }
            else if (unary->op == UNARYOP_NEGATE) {
                result = UnaryOp::negate(val);
                if (!result) {
                    return error(unary, "Operand of \'-\' operator is non-numerical");
                }
            }
            else if (unary->op == UNARYOP_COMPLEMENT) {
                result = UnaryOp::complement(val);
                if (!result) {
                    return error(unary, "Operand of \'~\' operator is not an integer");
                }
            }
            else if (unary->op == UNARYOP_NOT) {
                result = UnaryOp::logicalNot(val);
                if (!result) {
                    return error(unary, "Operand of \'!\' operator is not a boolean");
                }
            }
            else if (unary->op == UNARYOP_DEREFERENCE) {
                if (!val->type->isPtr()) {
                    return error(unary, "Attempting to dereference a non-pointer");
                }
                result = UnaryOp::dereference(val);
                if (!result) {
                    return error(unary, "Cannot dereference pointer to unknown/incomplete type");
                }
            }   
            return success();
        }
    }

    bool Analyzer::getBinaryExpr(const std::shared_ptr<BinaryExprNode>& binary) {
        if (binary->op == BINARYOP_RECAST) {
            if (!getType(binary->rhs, false)) {
                return error(binary, "Unable to obtain recast type");
            }
            auto ty = std::static_pointer_cast<Type>(result);
            if (!getValue(binary->lhs)) {
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

        if (!getValue(binary->lhs)) {
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
            result = BinaryOp::member(var, mem);
            if (!result) {
                return error(binary, "Member operation failed");
            }
            return success();
        }

        if (!getValue(binary->rhs)) {
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
            result = BinaryOp::element(var, rhs);
            if (!result) {
                return error(binary, "Element operation failed");
            }
        }

        else if (binary->op == BINARYOP_STR_CONCAT) {
            // Operands should ALWAYS be StringLiterals; no checks needed 
            auto lstr = std::static_pointer_cast<StringLiteral>(lhs);
            auto rstr = std::static_pointer_cast<StringLiteral>(rhs);
            result = BinaryOp::concat(lstr, rstr);
        }
        else if (binary->op == BINARYOP_ADD) {
            result = BinaryOp::add(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'+\' operation");
            }
        }
        else if (binary->op == BINARYOP_SUBTRACT) {
            result = BinaryOp::subtract(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'-\' operation");
            }
        }
        else if (binary->op == BINARYOP_MULTIPLY) {
            result = BinaryOp::multiply(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'*\' operation");
            }
        }
        else if (binary->op == BINARYOP_DIVIDE) {
            result = BinaryOp::divide(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'/\' operation");
            }
        }
        else if (binary->op == BINARYOP_MOD) {
            result = BinaryOp::remainder(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'%\' operation");
            }
        }
        else if (binary->op == BINARYOP_SHIFT_LEFT) {
            result = BinaryOp::shiftLeft(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'<<\' operation");
            }
        }
        else if (binary->op == BINARYOP_SHIFT_RIGHT) {
            result = BinaryOp::shiftRight(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'>>\' operation");
            }
        }
        else if (binary->op == BINARYOP_BIT_OR) {
            result = BinaryOp::bitOr(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'|\' operation");
            }
        }
        else if (binary->op == BINARYOP_BIT_AND) {
            result = BinaryOp::bitAnd(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'&\' operation");
            }
        }
        else if (binary->op == BINARYOP_BIT_XOR) {
            result = BinaryOp::bitXor(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'^\' operation");
            }
        }
        else if (binary->op == BINARYOP_LOGICAL_OR) {
            result = BinaryOp::logOr(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'||\' operation");
            }
        }
        else if (binary->op == BINARYOP_LOGICAL_AND) {
            result = BinaryOp::logAnd(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'&&\' operation");
            }
        }
        else if (binary->op == BINARYOP_EQUAL) {
            result = BinaryOp::equal(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'==\' operation");
            }
        }
        else if (binary->op == BINARYOP_NOT_EQUAL) {
            result = BinaryOp::unequal(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'!=\' operation");
            }
        }
        else if (binary->op == BINARYOP_GREATER) {
            result = BinaryOp::greater(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'>\' operation");
            }
        }
        else if (binary->op == BINARYOP_LESSER) {
            result = BinaryOp::lesser(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'<\' operation");
            }
        }
        else if (binary->op == BINARYOP_GREATER_EQUAL) {
            result = BinaryOp::greaterEqual(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'<=\' operation");
            }
        }
        else if (binary->op == BINARYOP_LESSER_EQUAL) {
            result = BinaryOp::lesserEqual(lhs, rhs);
            if (!result) {
                return error(binary, "Unable to perform \'&&\' operation");
            }
        }

        return success();
    }

}
