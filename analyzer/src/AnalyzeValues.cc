#include <llvm/IR/Constants.h>
#include <Analyzer.h>
#include <Literal.h>
#include <LiteralNode.h>
#include <UnaryOp.h>
#include <BinaryOp.h>
#include <PointerType.h>
#include <StructType.h>
#include <UnionType.h>

namespace avl {

    bool Analyzer::getValue(const std::shared_ptr<Node>& node) {
        if (node->kind == NODE_IDENTIFIER) {
            auto ident = std::static_pointer_cast<Identifier>(node);
            auto n = ident->name;
            if (gst->constants.find(n) != gst->constants.end()) {
                if (!gst->constants.find(n)->second) {
                    return error(ident, "Cannot create a complete representation of " + n);
                }
                result = gst->constants[n];
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
                gst->constants[n] = std::shared_ptr<Value>();
                if (!getValue(ast->representations[n]->node)) {
                    return error();
                }
                result = gst->constants[n] = std::static_pointer_cast<Value>(result);
            }
            else if (currentFunction && currentFunction->lst->isDefined(n)) {
                result = currentFunction->lst->getVariable(n);
            }
            else if (gst->variables.find(n) != gst->variables.end()) {
                result = gst->variables[n];
            }
            else if (gst->functions.find(n) != gst->functions.end()) {
                result = gst->functions[n];
            }
            else if (ast->declarations.find(n) != ast->declarations.end() ||
                     ast->definitions.find(n) != ast->definitions.end()) 
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
            return success();
        }

        if (node->kind != NODE_EXPRNODE) {
            return error();
        }

        auto expr = static_cast<ExprNode*>(node.get());
        if (expr->isLiteralNode()) {
            return literal(std::static_pointer_cast<ExprNode>(node));
        }
        else if (expr->is == EXPR_ASSIGN) {
            return binary(std::static_pointer_cast<ExprNode>(node));
        }
        else if (expr->is == EXPR_BINARY) {
            return binary(std::static_pointer_cast<ExprNode>(node));
        }
        else if (expr->is == EXPR_CALL) {
            return call(std::static_pointer_cast<CallExprNode>(node));
        }
        else if (expr->is == EXPR_UNARY) {
            return unary(std::static_pointer_cast<UnaryExprNode>(node));
        }

        return error();

    }

    bool Analyzer::literal(const std::shared_ptr<ExprNode>& expr) {
        if (expr->is == EXPR_INT) {
            auto intnode = static_cast<IntNode*>(expr.get());
            result = std::make_shared<IntLiteral>(intnode->literal);
        }
        else if (expr->is == EXPR_BOOL) {
            auto boolnode = static_cast<BoolNode*>(expr.get());
            result = std::make_shared<BoolLiteral>(boolnode->literal);
        }
        else if (expr->is == EXPR_REAL) {
            auto realnode = static_cast<RealNode*>(expr.get());
            result = std::make_shared<RealLiteral>(realnode->literal);
        }
        else if (expr->is == EXPR_CHAR) {
            auto charnode = static_cast<CharNode*>(expr.get());
            result = std::make_shared<CharLiteral>(charnode->literal);
        }
        else if (expr->is == EXPR_STRING) {
            auto strnode = static_cast<StringNode*>(expr.get());
            result = std::make_shared<StringLiteral>(strnode->literal);
        }
        else {
            return error();
        }
        return success();
    }

    bool Analyzer::unary(const std::shared_ptr<UnaryExprNode>& un) {
        if (un->op == UNARYOP_SIZE) {
            bool istype = false;
            if (!getValue(un->exp)) {
                if (hasErrors()) {
                    return error();
                }
                if (!getType(un->exp, false)) {
                    return error(un, "Unable to evaluate the operand of the size operation");
                }
                istype = true;
            }
            auto ty = istype ? std::static_pointer_cast<Type>(result) : std::static_pointer_cast<Value>(result)->type;
            if (ty->size() == 0) {
                return error(un, "Cannot perform size operation on this type");
            }
            result = UnaryOp::size(ty); // result should always be valid
            return success();
        }

        if (!getValue(un->exp)) {
            return error(un, "Unable to evaluate the operand of unary operation");
        }
        auto val = std::static_pointer_cast<Value>(result);

        if (un->op == UNARYOP_ADDRESS) {
            result = UnaryOp::address(val);
            if (!val->isInstance() && !(val->type->isInt() && val->isConst())) {
                return error(un, "Operand of address operation is neither an instance nor a constant integer");
            }
        }
        else if (un->op == UNARYOP_PLUS) {
            result = UnaryOp::plus(val);
            if (!result) {
                return error(un, "Operand of \'+\' operator is non-numerical");
            }
        }
        else if (un->op == UNARYOP_NEGATE) {
            result = UnaryOp::negate(val);
            if (!result) {
                return error(un, "Operand of \'-\' operator is non-numerical");
            }
        }
        else if (un->op == UNARYOP_COMPLEMENT) {
            result = UnaryOp::complement(val);
            if (!result) {
                return error(un, "Operand of \'~\' operator is not an integer");
            }
        }
        else if (un->op == UNARYOP_NOT) {
            result = UnaryOp::logicalNot(val);
            if (!result) {
                return error(un, "Operand of \'!\' operator is not a boolean");
            }
        }
        else if (un->op == UNARYOP_DEREFERENCE) {
            if (!val->type->isPtr()) {
                return error(un, "Attempting to dereference a non-pointer");
            }
            result = UnaryOp::dereference(val);
            if (!result) {
                return error(un, "Cannot dereference pointer to unknown/incomplete type");
            }
        }
        else {
            return error();
        }

        return success();
    }

    bool Analyzer::recast(const std::shared_ptr<BinaryExprNode>& binary) {
        if (!getType(binary->rhs, false)) {
            return error(binary, "Unable to obtain recast type");
        }
        auto ty = std::static_pointer_cast<Type>(result);
        if (!getValue(binary->lhs)) {
            return error(binary, "Unable to obtain expression to recast");
        }
        auto ex = std::static_pointer_cast<Value>(result);
        if (ty->isCompound()) {
            return error(binary, "Cannot recast to compound type");
        }
        if (ty->isFunction()) {
            return error(binary, "Cannot recast to function type");
        }
        result = BinaryOp::recast(ex, ty);
        if (!result) {
            return error(binary, "Recast failed"); // This should never happen
        }
        return success();
    }

    bool Analyzer::member(const std::shared_ptr<BinaryExprNode>& binary) {
        if (!getValue(binary->lhs)) {
            return error(binary, "Unable to evaluate the left side of the member operation");
        }
        auto lhs = std::static_pointer_cast<Value>(result);

        if (!lhs->isVar()) {
            return error(binary, "Need a variable to perform the member operation");
        }
        if (!lhs->type->isStruct() && !lhs->type->isUnion()) {
            return error(binary, "Variable type for the member operation must be struct or union");
        }
        auto memident = static_cast<Identifier*>(binary->rhs.get());
        auto var = std::static_pointer_cast<Variable>(lhs);
        result = BinaryOp::member(var, memident->name);
        if (!result) {
            return error(binary, "No member called " + memident->name);
        }
        return success();
    }

    bool Analyzer::element(const std::shared_ptr<BinaryExprNode>& binary) {
        if (!getValue(binary->lhs)) {
            return error(binary, "Unable to evaluate the left side of the element operation");
        }
        auto lhs = std::static_pointer_cast<Value>(result);
        if (!getValue(binary->rhs)) {
            return error(binary, "Unable to evaluate the right side of the element operation");
        }
        auto rhs = std::static_pointer_cast<Value>(result);

        if (lhs->is != VALUE_VAR) {
            return error(binary, "Need a variable to perform the element operation");
        }
        if (lhs->type->isPrimitive() || lhs->type->isUnknown() ||
            lhs->type->isFunction()  || lhs->type->isVoid())
        {
            return error(binary, "Attempting to perform element operation on this type");
        }
        if (!rhs->type->isInt()) {
            return error(binary, "Element index is not an integer");
        }
        if (lhs->type->isStruct() || lhs->type->isUnion()) {
            if (!rhs->isConst()) {
                return error(binary, "Element index is not an integer constant");
            }
            auto ci = llvm::cast<llvm::ConstantInt>(rhs->val());
            if (ci->isNegative()) {
                return error(binary, "Element index cannot be negative");
            }
            auto idx = ci->getZExtValue();
            if (lhs->type->isStruct()) {
                auto st = static_cast<StructType*>(lhs->type.get());
                if (idx >= st->members.size()) {
                    return error(binary, "Element index beyond the number of struct members");
                }
            }
            else {
                auto ut = static_cast<UnionType*>(lhs->type.get());
                if (idx >= ut->members.size()) {
                    return error(binary, "Element index beyond the number of union members");
                }
            }
        }
        else if (lhs->type->isPtr()) {
            auto pt = static_cast<PointerType*>(lhs->type.get());
            if (pt->points_to->isUnknown()) {
                return error(binary, "Element operation cannot be performed on a pointer to unknown type");
            }
        }
        auto var = std::static_pointer_cast<Variable>(lhs);
        result = BinaryOp::element(var, rhs);
        if (!result) {
            return error(binary, "Element operation failed"); // This should never happen
        }
        return success();
    }

    bool Analyzer::binary(const std::shared_ptr<ExprNode>& expr) {

        uint16_t op;
        std::string opstr;
        std::shared_ptr<Node> lhs_node;
        std::shared_ptr<Node> rhs_node;

        if (expr->is == EXPR_BINARY) {
            auto bin = std::static_pointer_cast<BinaryExprNode>(expr);
            op = bin->op;
            if (op == BINARYOP_RECAST) {
                return recast(bin);
            }
            else if (op == BINARYOP_MEMBER) {
                return member(bin);
            }
            else if (op == BINARYOP_ELEMENT) {
                return element(bin);
            }
            opstr = ExprNode::binopstring(op);
            lhs_node = bin->lhs;
            rhs_node = bin->rhs;
        }
        else if (expr->is == EXPR_ASSIGN) {
            auto assex = std::static_pointer_cast<AssignExprNode>(expr);
            op = assex->op;
            opstr = ExprNode::assopstring(op);
            lhs_node = assex->lhs;
            rhs_node = assex->rhs;
        }
        else {
            return error();
        }

        if (!getValue(lhs_node)) {
            return error(lhs_node, "Unable to evaluate the left side of the expression");
        }
        auto lhs = std::static_pointer_cast<Value>(result);

        if (expr->is == EXPR_ASSIGN) {
            if (lhs->is != VALUE_VAR) {
                return error(lhs_node, "Left side of the assign expression is not instantiable");
            }
            if (op == ASSIGNOP_ASSIGN) {
                return assign(std::static_pointer_cast<Variable>(lhs), rhs_node);
            }
        }

        if (!getValue(rhs_node)) {
            return error(rhs_node, "Unable to evaluate the right side of the expression");
        }
        auto rhs = std::static_pointer_cast<Value>(result);

        if (*lhs->type != *rhs->type) {
            return error(expr, "Operands of \'" + opstr + "\' operation must have the same type");
        }

        if (op == BINARYOP_ADD        || 
            op == BINARYOP_SUBTRACT   || 
            op == BINARYOP_MULTIPLY   ||
            op == BINARYOP_DIVIDE     ||
            op == BINARYOP_REMAINDER  ||
            op == ASSIGNOP_ADD_ASSIGN || 
            op == ASSIGNOP_SUB_ASSIGN || 
            op == ASSIGNOP_MUL_ASSIGN || 
            op == ASSIGNOP_DIV_ASSIGN || 
            op == ASSIGNOP_REM_ASSIGN || 
            op == ASSIGNOP_REM_ASSIGN) 
        {
            if (!lhs->type->isInt() && !lhs->type->isReal()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have integer or real type");
            }
            if (op == BINARYOP_DIVIDE     || op == BINARYOP_REMAINDER  || 
                op == ASSIGNOP_DIV_ASSIGN || op == ASSIGNOP_REM_ASSIGN) 
            {
                switch (BinaryOp::checkValidDivision(lhs, rhs)) {
                    case UB_DIV_ZERO     : return error(expr, "Divisor of \'" + opstr + "\' operation is zero");
                    case UB_DIV_OVERFLOW : return error(expr, "\'" + opstr + "\' operation has signed overflow");
                }
            }
            switch (op) {
                case BINARYOP_ADD        : result = BinaryOp::add(lhs, rhs);       break;
                case BINARYOP_SUBTRACT   : result = BinaryOp::subtract(lhs, rhs);  break;
                case BINARYOP_MULTIPLY   : result = BinaryOp::multiply(lhs, rhs);  break;
                case BINARYOP_DIVIDE     : result = BinaryOp::divide(lhs, rhs);    break;
                case BINARYOP_REMAINDER  : result = BinaryOp::remainder(lhs, rhs); break;
                case ASSIGNOP_ADD_ASSIGN : result = BinaryOp::add(lhs, rhs);       break;
                case ASSIGNOP_SUB_ASSIGN : result = BinaryOp::subtract(lhs, rhs);  break;
                case ASSIGNOP_MUL_ASSIGN : result = BinaryOp::multiply(lhs, rhs);  break;
                case ASSIGNOP_DIV_ASSIGN : result = BinaryOp::divide(lhs, rhs);    break;
                case ASSIGNOP_REM_ASSIGN : result = BinaryOp::remainder(lhs, rhs); break;
            }
        }
        else if (op == BINARYOP_SHIFT_LEFT  || 
                 op == BINARYOP_SHIFT_RIGHT ||
                 op == BINARYOP_BIT_OR      || 
                 op == BINARYOP_BIT_AND     || 
                 op == BINARYOP_BIT_XOR     || 
                 op == ASSIGNOP_AND_ASSIGN  || 
                 op == ASSIGNOP_OR_ASSIGN   || 
                 op == ASSIGNOP_XOR_ASSIGN  ||
                 op == ASSIGNOP_BIT_RIGHT_ASSIGN || 
                 op == ASSIGNOP_BIT_LEFT_ASSIGN) 
        {
            if (!lhs->type->isInt()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have integer type");
            }
            switch (op) {
                case BINARYOP_SHIFT_LEFT       : result = BinaryOp::shiftLeft(lhs, rhs);  break;
                case BINARYOP_SHIFT_RIGHT      : result = BinaryOp::shiftRight(lhs, rhs); break;
                case BINARYOP_BIT_OR           : result = BinaryOp::bitOr(lhs, rhs);      break;
                case BINARYOP_BIT_AND          : result = BinaryOp::bitAnd(lhs, rhs);     break;
                case BINARYOP_BIT_XOR          : result = BinaryOp::bitXor(lhs, rhs);     break;
                case ASSIGNOP_BIT_LEFT_ASSIGN  : result = BinaryOp::shiftLeft(lhs, rhs);  break;
                case ASSIGNOP_BIT_RIGHT_ASSIGN : result = BinaryOp::shiftRight(lhs, rhs); break;
                case ASSIGNOP_OR_ASSIGN        : result = BinaryOp::bitOr(lhs, rhs);      break;
                case ASSIGNOP_AND_ASSIGN       : result = BinaryOp::bitAnd(lhs, rhs);     break;
                case ASSIGNOP_XOR_ASSIGN       : result = BinaryOp::bitXor(lhs, rhs);     break;
            }
        }
        else if (op == BINARYOP_LOGICAL_OR || op == BINARYOP_LOGICAL_AND) {
            if (!lhs->type->isBool()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have boolean type");
            }
            result = (op == BINARYOP_LOGICAL_OR ? BinaryOp::logOr(lhs, rhs) : BinaryOp::logAnd(lhs, rhs));
        }
        else if (op == BINARYOP_EQUAL || op == BINARYOP_NOT_EQUAL) {
            if (!lhs->type->isPrimitive() && !lhs->type->isPtr()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have numerical or pointer type");
            }
            result = (op == BINARYOP_EQUAL ? BinaryOp::equal(lhs, rhs) : BinaryOp::unequal(lhs, rhs));
        }
        else if (op == BINARYOP_GREATER || op == BINARYOP_LESSER ||
                 op == BINARYOP_GREATER_EQUAL || op == BINARYOP_LESSER_EQUAL) 
        {
            if (!lhs->type->isInt() && !lhs->type->isReal() && !lhs->type->isPtr()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have integer, real or pointer type");
            }
            switch (op) {
                case BINARYOP_GREATER       : result = BinaryOp::greater(lhs, rhs);       break;
                case BINARYOP_LESSER        : result = BinaryOp::lesser(lhs, rhs);        break;
                case BINARYOP_GREATER_EQUAL : result = BinaryOp::greaterEqual(lhs, rhs);  break;
                case BINARYOP_LESSER_EQUAL  : result = BinaryOp::lesserEqual(lhs, rhs);   break;
            }
        }
        else {
            return error();
        }

        if (expr->is != EXPR_ASSIGN) {
            return success();
        }

        auto lvar = std::static_pointer_cast<Variable>(lhs);
        auto rval = std::static_pointer_cast<Value>(result);
        result = BinaryOp::assign(lvar, rval);
        if (!result) {
            return error();
        }
        return success();
    }

    bool Analyzer::assign(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

        if (rval->kind == NODE_EXPRNODE) {
            auto expr = static_cast<ExprNode*>(rval.get());
            if (expr->is == EXPR_CALL) {
                return call(std::static_pointer_cast<CallExprNode>(rval), var);
            }
        }

        if (!getValue(rval)) {
            return error(rval, "Unable to evaluate the right side of the expression");
        }
        auto ex = std::static_pointer_cast<Value>(result);
        ex = BinaryOp::recastImplicit(ex, var->type);
        if (!ex) {
            return error(rval, "Attempting to assign incompatible type to variable");
        }
        result = BinaryOp::assign(var, ex);
        return success();
        
    }

}
