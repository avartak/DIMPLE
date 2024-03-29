#include <Translator/Translator.h>
#include <AST/LiteralNode.h>
#include <IR/Literal.h>
#include <IR/UnaryOp.h>
#include <IR/BinaryOp.h>
#include <IR/PointerType.h>
#include <IR/StructType.h>
#include <IR/UnionType.h>
#include <IR/CodeBlock.h>
#include <IR/LST.h>

namespace dmp {

    bool Translator::getValue(const std::shared_ptr<Node>& node) {
        if (node->kind == NODE_IDENTIFIER) {
            auto ident = std::static_pointer_cast<Identifier>(node);
            auto n = ident->name;
            if (n == "main") {
                return error(ident, "Invalid use of \'main\'");
            }
            if (currentFunction && currentFunction->lst->isDefined(n)) {
                result = currentFunction->lst->getInstance(n);
            }
            else if (ast->representations.find(n) != ast->representations.end() ||
                     ast->declarations.find(n) != ast->declarations.end() ||
                     ast->definitions.find(n) != ast->definitions.end()) 
            {
                return getConstRep(ident) || getGlobalInstance(ident);
            }
            else {
                return error(ident, "Unable to decipher identifer " + n);
            }
            return success();
        }

        if (node->kind != NODE_EXPRNODE) {
            return error();
        }

        auto expr = std::static_pointer_cast<ExprNode>(node);
        switch (expr->is) {
            case EXPR_ASSIGN : return binary(expr);
            case EXPR_BINARY : return binary(expr);
            case EXPR_CALL   : return call(std::static_pointer_cast<CallExprNode>(node));
            case EXPR_UNARY  : return unary(std::static_pointer_cast<UnaryExprNode>(node));
            default          : return literal(expr);
        }
    }

    bool Translator::getConstRep(const std::shared_ptr<Identifier>& ident) {

        auto n = ident->name;
        if (gst->constants.find(n) != gst->constants.end()) {
            if (!gst->constants[n]) {
                return error(ident, "Cannot create a complete representation of " + n);
            }
            result = gst->constants[n];
            return success();
        }
        else if (ast->representations.find(n) != ast->representations.end()) {
            if (ast->getNonSynonymRepNode(ident)->kind != NODE_EXPRNODE) {
                return error();
            }
            gst->constants[n] = std::shared_ptr<Value>();
            if (!getValue(ast->representations[n]->node)) {
                return error();
            }
            result = gst->constants[n] = std::static_pointer_cast<Value>(result);
            return success();
        }
        return error();
    }

    bool Translator::getGlobalInstance(const std::shared_ptr<Identifier>& ident) {

        const auto& n = ident->name;

        if (gst->functions.find(n) != gst->functions.end()) {
            if (!gst->functions[n]) {
                return error("Unable to completely define " + n);
            }
            result = gst->functions[n];
            return success();
        }
        else if (gst->variables.find(n) != gst->variables.end()) {
            if (!gst->variables[n]) {
                return error("Unable to completely define " + n);
            }
            result = gst->variables[n];
            return success();
        }

        if (ast->declarations.find(n) == ast->declarations.end() &&
            ast->definitions.find(n) == ast->definitions.end())
        {
            return error();
        }

        std::shared_ptr<Node> tnode;
        uint16_t storage = (ast->definitions.find(n) != ast->definitions.end() ? ast->definitions[n]->storage : STORAGE_EXTERNAL);

        if (storage == STORAGE_REFERENCE) {
            return getGlobalRef(ident);
        }

        if (ast->declarations.find(n) != ast->declarations.end()) {
            tnode = ast->declarations[n]->node;
        }
        else {
            tnode = ast->definitions[n]->type;
        }

        if (!tnode) {
            return getGlobalVar(ident, storage, nullptr);
        }

        auto tynode = tnode;
        if (tynode->kind == NODE_IDENTIFIER) {
            tynode = ast->getNonSynonymRepNode(std::static_pointer_cast<Identifier>(tnode));
        }
        if (tynode->kind == NODE_IDENTIFIER) {
            return error(tynode, "\'" + static_cast<Identifier*>(tynode.get())->name + "\' is not a representation");
        }
        if (tynode->kind != NODE_TYPENODE) {
            return error(tnode, "\'" +  static_cast<Identifier*>(tnode.get())->name + "\' is not a type");
        }
        if (static_cast<TypeNode*>(tynode.get())->isFunction()) {
            gst->functions[n] = std::shared_ptr<Function>();
        }
        else {
            gst->variables[n] = std::shared_ptr<Variable>();
        }

        if (!getType(tnode)) {
            return error(ident, "Unable to determine type of " + n);
        }
        auto type = std::static_pointer_cast<Type>(result);
        if (!type->isComplete()) {
            return error(ident, "Type of " + n + " is not completely defined");
        }

        return ( type->isFunction() ? getFunction(ident, storage, type) : getGlobalVar(ident, storage, type) );
    }

    bool Translator::getGlobalRef(const std::shared_ptr<Identifier>& ident) {

        const auto& n = ident->name;
        const auto& defn = ast->definitions[n];
        const auto& def = defn->def;

        // I can't think of a better way to 'mark' the reference than declaring it both as a variable and a function
        gst->functions[n] = std::shared_ptr<Function>();
        gst->variables[n] = std::shared_ptr<Variable>();

        if (!getValue(def)) {
            return error(def, "Unable to determine the referee of global reference " + n);
        }
        auto referee = std::static_pointer_cast<Value>(result);
        if (!referee->isInstance()) {
            return error(def, "Referee of " + n + " is not an instance");
        }
        if (referee->isVar()) {
            auto var = std::make_shared<Variable>(STORAGE_REFERENCE, n, referee->type);
            var->llvm_value = static_cast<Variable*>(referee.get())->ptr();
            result = gst->variables[n] = var;
            gst->functions.erase(gst->functions.find(n));
        }
        else {
            auto func = std::make_shared<Function>(STORAGE_REFERENCE, n, referee->type);
            func->llvm_value = static_cast<Function*>(referee.get())->ptr();
            result = gst->functions[n] = func;
            gst->variables.erase(gst->variables.find(n));
        }

        return success();
    }

    bool Translator::literal(const std::shared_ptr<ExprNode>& expr) {
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

    bool Translator::unary(const std::shared_ptr<UnaryExprNode>& un) {
        if (un->op == UNARYOP_SIZE) {
            bool istype = false;
            if (!getValue(un->exp)) {
                if (hasErrors()) {
                    return error();
                }
                if (!getType(un->exp)) {
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

        std::string err = "";
        if (un->op == UNARYOP_ADDRESS) {
            result = UnaryOp::address(val);
            err = "Operand of address operation is neither an instance nor a constant integer";
        }
        else if (un->op == UNARYOP_PLUS) {
            result = UnaryOp::plus(val);
            err = "Operand of \'+\' operator is non-numerical";
        }
        else if (un->op == UNARYOP_NEGATE) {
            result = UnaryOp::negate(val);
            err = "Operand of \'-\' operator is non-numerical";
        }
        else if (un->op == UNARYOP_COMPLEMENT) {
            result = UnaryOp::complement(val);
            err = "Operand of \'~\' operator is not an integer";
        }
        else if (un->op == UNARYOP_NOT) {
            result = UnaryOp::logicalNot(val);
            err = "Operand of \'!\' operator is not a boolean";
        }
        else if (un->op == UNARYOP_DEREFERENCE) {
            if (!val->type->isPtr()) {
                return error(un, "Attempting to dereference a non-pointer");
            }
            result = UnaryOp::dereference(val);
            err = "Cannot dereference pointer to unknown/incomplete type";
        }
        else {
            return error();
        }

        return (result ? success() : error(un, err));
    }

    bool Translator::recast(const std::shared_ptr<BinaryExprNode>& binary) {
        if (!getType(binary->rhs)) {
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

    bool Translator::member(const std::shared_ptr<BinaryExprNode>& binary) {
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

    bool Translator::element(const std::shared_ptr<BinaryExprNode>& binary) {
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
        if (!lhs->type->isCompound() && !lhs->type->isPtr()) {
            return error(binary, "Cannot perform element operation on this type");
        }
        if (!rhs->type->isInt()) {
            return error(binary, "Element index is not an integer");
        }
        if (lhs->type->isStruct() || lhs->type->isUnion()) {
            if (!rhs->isConstNoRelocation()) {
                return error(binary, "Element index is not a determinate constant");
            }
            if (!rhs->isConstNonNegativeInt()) {
                return error(binary, "Element index is not a non-negative integer constant");
            }
            auto idx = rhs->getUInt64ValueOrZero();
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
        else if (lhs->type->isArray()) {
            if (rhs->isConst()) {
                if (!rhs->isConstNoRelocation()) {
                    return error(binary, "Element index is not a determinate constant");
                }
                if (!rhs->isConstNonNegativeInt()) {
                    return error(binary, "Element index is not a non-negative integer constant");
                }
                auto idx = rhs->getUInt64ValueOrZero();
                auto at = static_cast<ArrayType*>(lhs->type.get());
                if (idx >= at->nelements) {
                    return error(binary, "Element index beyond the array size");
                }
            }
        }
        auto var = std::static_pointer_cast<Variable>(lhs);
        result = BinaryOp::element(var, rhs);
        if (!result) {
            return error(binary, "Element operation failed"); // This should never happen
        }
        return success();
    }

    bool Translator::binary(const std::shared_ptr<ExprNode>& expr) {

        uint16_t op;
        std::string opstr;
        std::shared_ptr<Node> lhs_node;
        std::shared_ptr<Node> rhs_node;

        if (expr->is == EXPR_BINARY) {
            auto bin = std::static_pointer_cast<BinaryExprNode>(expr);
            op = bin->op;
            switch (op) {
                case BINARYOP_RECAST  : return recast(bin);
                case BINARYOP_MEMBER  : return member(bin);
                case BINARYOP_ELEMENT : return element(bin);
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

        if ((op == BINARYOP_LOGICAL_OR || op == BINARYOP_LOGICAL_AND) && currentFunction) {
  
            if (!lhs->type->isBool()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have boolean type");
            }

            auto res = std::make_shared<Variable>(STORAGE_LOCAL, "", std::make_shared<PrimitiveType>(TYPE_BOOL));
            res->declare();
            BinaryOp::assign(res, lhs);

            auto lres = (op == BINARYOP_LOGICAL_OR ? UnaryOp::isTrue(lhs) : UnaryOp::isFalse(lhs));

            auto ifBB    = std::make_shared<CodeBlock>();
            auto mergeBB = std::make_shared<CodeBlock>();

            CodeBlock::branch(lres, mergeBB, ifBB);
            CodeBlock::insert(ifBB);

            if (!getValue(rhs_node)) {
                return error(rhs_node, "Unable to evaluate the right side of the expression");
            }
            auto rhs = std::static_pointer_cast<Value>(result);
            if (!rhs->type->isBool()) {
                return error(expr, "Operands of \'" + opstr + "\' operation must have boolean type");
            }
            BinaryOp::assign(res, rhs);

            CodeBlock::jump(mergeBB);
            CodeBlock::insert(mergeBB);

            result = res;
            return success();
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

    bool Translator::assign(const std::shared_ptr<Variable>& var, const std::shared_ptr<Node>& rval) {

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
