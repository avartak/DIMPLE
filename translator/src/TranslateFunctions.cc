#include <Translator.h>
#include <FunctionType.h>
#include <VoidType.h>
#include <BinaryOp.h>
#include <FunctionOp.h>
#include <Statement.h>
#include <CodeBlock.h>
#include <LST.h>
#include <Globals.h>

namespace avl {

    bool Translator::getFunction(const std::shared_ptr<Identifier>& ident, uint16_t storage, const std::shared_ptr<Type>& type) {

        const auto& n = ident->name;

        if (n == "main") {
            auto ftype = static_cast<FunctionType*>(type.get());
            bool sigisgood = true;
            if (storage != STORAGE_EXTERNAL) {
                return error(ident, "\'main\' function should be \'extern\'");
            }
            if (ftype->ret->is != TYPE_INT32) {
                sigisgood = false;
            }
            if (ftype->args.size() != 2) {
                sigisgood = false;
            }
            if (ftype->args[0].type->is != TYPE_INT32) {
                sigisgood = false;
            } 
            if (!ftype->args[1].type->isPtr()) {
                sigisgood = false;
            }
            auto pt = static_cast<PointerType*>(ftype->args[1].type.get())->points_to;
            if (!pt->isPtr()) {
                sigisgood = false;
            }
            auto ppt = static_cast<PointerType*>(pt.get())->points_to;
            if (ppt->is != TYPE_INT8) {
                sigisgood = false;
            }
            if (!sigisgood) {
                return error(ident, "Incorrect signature of \'main\' function");
            }
        }

        gst->functions[n] = std::make_shared<Function>(storage, n, type);
        gst->functions[n]->declare();
        if (ast->definitions.find(n) != ast->definitions.end()) {
            if (!defineFunction(ast->definitions[n])) {
                return error();
            }
            gst->functions[n] = std::static_pointer_cast<Function>(result);
        }

        result = gst->functions[n];
        return success();
    }

    bool Translator::defineFunction(const std::shared_ptr<DefineStatement>& defn) {

        auto cf = currentFunction;
        if (cf) {
            cf->freeze();
        }
        currentFunction = gst->functions[defn->name->name];
        currentFunction->init();

        if (defn->def->kind == NODE_NULLINIT) {
            auto ft = static_cast<FunctionType*>(currentFunction->type.get());
            if (!ft->ret->isVoid()) {
                return error(defn->name, "Function with a non-void return type cannot have an empty body");
            }
            FunctionOp::ret(currentFunction, nullptr);
        }
        else if (defn->def->kind == NODE_STATEMENT) {
            auto stmt = static_cast<Statement*>(defn->def.get());
            if (stmt->is != BLOCK_FUNCTION) {
                return error(stmt, "Invalid function body");
            }
            if (!defineBlock(std::static_pointer_cast<BlockNode>(defn->def))) {
                return error(defn->name, "Unable to define \'" + defn->name->name + "\'");
            }
            if (!currentFunction->checkTerminations()) {
                return error(defn->name, "Function \'" + defn->name->name + "\' does not always return");
            }
            // Possible to simplify the IR here: currentFunction->simplify();
        }
        else {
            return error(defn->def, "Invalid function body");
        }

        result = currentFunction;
        if (cf) {
            cf->resume();
        }
        currentFunction = cf;
        return success();
    }

    bool Translator::call(const std::shared_ptr<CallExprNode>& callex, const std::shared_ptr<Variable>& retv) {

        if (!getValue(callex->func)) {
            return error(callex->func, "Unable to construct the function of the call expression");
        }
        auto fval = static_cast<Value*>(result.get());
        if (fval->is != VALUE_FUNC) {
            return error(callex->func, "A function call is placed on an instance/value that is not a function");
        }
        auto func = std::static_pointer_cast<Function>(result);
        auto ft = static_cast<FunctionType*>(func->type.get());
        const auto& args = callex->args;

        if (args.size() < ft->args.size()) {
            return error("Function call has fewer arguments that required");
        }
        else if (args.size() > ft->args.size()) {
            return error("Function call has more arguments that required");
        }

        std::vector<std::shared_ptr<Value> > argvals;
        for (std::size_t i = 0; i < args.size(); i++) {
            if (!getValue(args[i])) {
                return error(args[i], "Unable to construct function argument " + std::to_string(i+1));
            }
            argvals.push_back(std::static_pointer_cast<Value>(result));
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            argvals[i] = BinaryOp::recastImplicit(argvals[i], ft->args[i].type);
            if (!argvals[i]) {
                return error(args[i], "Function argument " + std::to_string(i+1) + " has inconsistent type");
            }
            if (ft->args[i].passByRef() && !argvals[i]->isVar()) {
                return error(args[i], "Function argument " + std::to_string(i+1) + " must be an instance (variable)");
            }
        }

        result = bool(retv) ? FunctionOp::call(func, argvals, retv) : FunctionOp::call(func, argvals);
        if (!result) {
            return error(callex, "Function call failed"); // This should never happen
        }
        return success();
    }

    bool Translator::ret(const std::shared_ptr<ReturnStatement>& retstat) {
        auto ft = static_cast<FunctionType*>(currentFunction->type.get());
        if (!retstat->val && !ft->ret->isVoid()) {
            return error(retstat, "This function does not return void");
        }
        if (retstat->val && ft->ret->isVoid()) {
            return error(retstat, "This function returns void");
        }

        if (ft->ret->isVoid()) {
            FunctionOp::ret(currentFunction, nullptr);
            return success();
        }

        if (!getValue(retstat->val)) {
            return error(retstat, "Unable to determine return value");
        }
        auto retval = std::static_pointer_cast<Value>(result);
        retval = BinaryOp::recastImplicit(retval, ft->ret);
        if (!retval) {
            return error(retstat, "Return value is incompatible with the function return type");
        }

        result = nullptr;
        return FunctionOp::ret(currentFunction, retval); // This should never be false
    }

    bool Translator::defineLocalVar(const std::shared_ptr<DefineStatement>& definition) {

        const auto& n = definition->name->name;

        auto ft = static_cast<FunctionType*>(currentFunction->type.get());
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            if (ft->args[i].name && ft->args[i].name->name == n) {
                return error(definition->name, "Cannot define variable with argument name " + n);
            }
        }
        if (currentFunction->lst->isDefinedInThisScope(n)) {
            return error(definition->name, "Cannot define variable with name " + n + ". Symbol name is already in use in this scope");
        }
        if (ast->representations.find(n) != ast->representations.end()) {
            return error(definition->name, "\'" + n + "\' is also defined as a representation");
        }

        std::shared_ptr<Variable> var;
        if (!definition->type) {
            if (!getValue(definition->def)) {
                return error(definition->def, "Unable to determine the initial value of " + n);
            }
            auto init = std::static_pointer_cast<Value>(result);
            var = std::make_shared<Variable>(STORAGE_LOCAL, n, init->type);
            var->declare();
            BinaryOp::assign(var, init);
        }
        else {
            if (!getType(definition->type)) {
                return error(definition->type, "Unable to determine the type of " + n);
            }
            auto type = std::static_pointer_cast<Type>(result);
            if (type->isFunction()) {
                return error(definition, "Cannot use a function type to define a variable");
            }
            if (!type->isComplete()) {
                return error(definition->type, "Variable type is not completely defined");
            }
            var = std::make_shared<Variable>(STORAGE_LOCAL, n, type);
            if (!initLocal(var, definition)) {
                return error(definition->name, "Unable to initialize variable " + n);
            }
        }

        currentFunction->lst->variables[n] = var;
        result = var;
        return success();
    }

    bool Translator::defineLocalRef(const std::shared_ptr<DefineStatement>& definition) {

        const auto& n = definition->name->name;

        auto ft = static_cast<FunctionType*>(currentFunction->type.get());
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            if (ft->args[i].name && ft->args[i].name->name == n) {
                return error(definition->name, "Cannot define reference with argument name " + n);
            }
        }
        if (currentFunction->lst->isDefinedInThisScope(n)) {
            return error(definition->name, "Cannot define reference with name " + n + ". Symbol name is already in use in this scope");
        }
        if (ast->representations.find(n) != ast->representations.end()) {
            return error(definition->name, "\'" + n + "\' is also defined as a representation");
        }

        std::shared_ptr<Variable> var;
        if (!getValue(definition->def)) {
            return error(definition->def, "Unable to determine the referee of " + n);
        }
        auto referee = std::static_pointer_cast<Value>(result);
        if (!referee->isInstance()) {
            return error(definition->def, "Referee of " + n + " is not an instance");
        }
        if (referee->isVar()) {
            auto var = std::make_shared<Variable>(STORAGE_REFERENCE, n, referee->type);
            var->llvm_value = static_cast<Variable*>(referee.get())->ptr();
            currentFunction->lst->variables[n] = var;
            result = var;
        }
        else {
            auto func = std::make_shared<Function>(STORAGE_REFERENCE, n, referee->type);
            func->llvm_value = static_cast<Function*>(referee.get())->ptr();
            currentFunction->lst->functions[n] = func;
            result = func;
        }

        return success();
    }

    bool Translator::defineBlock(const std::shared_ptr<BlockNode>& block, std::shared_ptr<CodeBlock> start, std::shared_ptr<CodeBlock> end) {

        for (const auto& statement : block->body) {
            if (statement->is == STATEMENT_CONTINUE) {
                if (!start) {
                    return error(statement, "\'continue\' statement cannot be used in this block");
                }
                CodeBlock::jump(start);
            }
            else if (statement->is == STATEMENT_BREAK) {
                if (!end) {
                    return error(statement, "\'break\' statement cannot be used in this block");
                }
                CodeBlock::jump(end);
            }
            else if (statement->is == STATEMENT_RETURN) {
                if (!ret(std::static_pointer_cast<ReturnStatement>(statement))) {
                    return error();
                }
            }
            else if (statement->is == STATEMENT_CALL) {
                auto callstat = static_cast<CallStatement*>(statement.get());
                if (!getValue(callstat->exp)) {
                    return error();
                }
            }
            else if (statement->is == STATEMENT_ASSIGN) {
                auto assignstat = static_cast<AssignStatement*>(statement.get());
                if (!getValue(assignstat->exp)) {
                    return error();
                }
            }
            else if (statement->is == STATEMENT_DEFINE) {
                auto defstat = std::static_pointer_cast<DefineStatement>(statement);
                if (defstat->storage == STORAGE_REFERENCE) {
                    if (!defineLocalRef(defstat)) {
                        return error();
                    }
                }
                else {
                    if (!defineLocalVar(defstat)) {
                        return error();
                    }
                }
            }
            else if (statement->is == BLOCK_IF) {
                if (!defineIfBlock(std::static_pointer_cast<BlockNode>(statement), start, end)) {
                    return error();
                }
            }
            else if (statement->is == BLOCK_LOOP) {
                if (!defineLoopBlock(std::static_pointer_cast<BlockNode>(statement))) {
                    return error();
                }
            }
            else {
                return error(statement, "Unable to identify the statement");
            }
        }

        return success();
    }

    bool Translator::defineIfBlock(const std::shared_ptr<BlockNode>& block, std::shared_ptr<CodeBlock> start, std::shared_ptr<CodeBlock> end) {

        auto mergeBB = std::make_shared<CodeBlock>();

        for (std::size_t i = 0; i < block->body.size(); i++) {
            
            auto ifblock = std::static_pointer_cast<CondBlockNode>(block->body[i]);
            auto nextBB = std::make_shared<CodeBlock>();
            
            if (ifblock->condition) {
                if (!getValue(ifblock->condition)) { 
                    return error(ifblock->condition, "Unable to evaluate the if condition");
                }
                auto ex = std::static_pointer_cast<Value>(result);
                if (!ex->type->isBool()) {
                    return error(ifblock->condition, "if condition statement does not evaluate to a boolean");
                }
                
                auto ifBB = std::make_shared<CodeBlock>();
                CodeBlock::branch(ex, ifBB, (i != block->body.size()-1 ? nextBB : mergeBB));
                CodeBlock::insert(ifBB);
            }
            
            auto newlst = std::make_shared<LST>(*currentFunction);
            if (!defineBlock(ifblock, start, end)) {
                return error();
            }
            currentFunction->descope();
            
            CodeBlock::jump(mergeBB);
            if (i != block->body.size()-1) {
                CodeBlock::insert(nextBB);
            }
        }   
        
        CodeBlock::insert(mergeBB);
        return success();
    }

    bool Translator::defineLoopBlock(const std::shared_ptr<BlockNode>& block) {

        auto newlst = std::make_shared<LST>(*currentFunction);

        const auto& init = block->body[0];
        const auto& loop = block->body[1];
        const auto& updt = block->body[2];
        auto loopblock = std::static_pointer_cast<CondBlockNode>(loop);

        // Attempting to make the loop IR as canonical as possible
        auto phdrBB  = std::make_shared<CodeBlock>(); // Preheader
        auto loopBB  = std::make_shared<CodeBlock>(); // Header
        auto updtBB  = std::make_shared<CodeBlock>(); // Update of iteratable and termination check 
        auto exitBB  = std::make_shared<CodeBlock>(); // Exit block
        auto mergeBB = std::make_shared<CodeBlock>(); // Merge block out of the loop 

        if (init) {
            if (init->is == STATEMENT_DEFINE) {
                auto defstat = std::static_pointer_cast<DefineStatement>(init);
                if (defstat->storage == STORAGE_REFERENCE) {
                    if (!defineLocalRef(defstat)) {
                        return error();
                    }
                }
                else {
                    if (!defineLocalVar(defstat)) {
                        return error();
                    }
                }
            }
            else if (init->is == STATEMENT_ASSIGN) {
                auto assignstat = static_cast<AssignStatement*>(init.get());
                if (!getValue(assignstat->exp)) {
                    return error();
                }
            }
            else if (init->is == STATEMENT_CALL) {
                auto callstat = static_cast<CallStatement*>(init.get());
                if (!getValue(callstat->exp)) {
                    return error();
                }
            }
            else {
                return error(init, "Invalid loop initializer statement");
            }
        }

        if (loopblock->condition) {
            if (!getValue(loopblock->condition)) {
                return error(loopblock->condition, "Unable to evaluate the loop condition");
            }
            auto ex = std::static_pointer_cast<Value>(result);
            if (!ex->type->isBool()) {
                return error(loopblock->condition, "Loop condition statement does not evaluate to a boolean");
            }
            CodeBlock::branch(ex, phdrBB, mergeBB);
        }
        else {
            CodeBlock::jump(phdrBB);
        }

        CodeBlock::jump(phdrBB);
        CodeBlock::insert(phdrBB);
        CodeBlock::jump(loopBB);

        CodeBlock::insert(loopBB);
        if (!defineBlock(loopblock, updtBB, exitBB)) {
            return error();
        }
        CodeBlock::jump(updtBB);

        CodeBlock::insert(updtBB);
        if (updt) {
            if (updt->is == STATEMENT_ASSIGN) {
                auto assignstat = static_cast<AssignStatement*>(updt.get());
                if (!getValue(assignstat->exp)) {
                    return error();
                }
            }
            else if (updt->is == STATEMENT_CALL) {
                auto callstat = static_cast<CallStatement*>(updt.get());
                if (!getValue(callstat->exp)) {
                    return error();
                }
            }
            else {
                return error(updt, "Invalid loop update statement");
            }
        }
        if (loopblock->condition) {
            getValue(loopblock->condition);
            auto ex = std::static_pointer_cast<Value>(result);
            CodeBlock::branch(ex, loopBB, exitBB);
        }
        else {
            CodeBlock::jump(loopBB);
        }

        CodeBlock::insert(exitBB);
        CodeBlock::jump(mergeBB);
        CodeBlock::insert(mergeBB);

        currentFunction->descope();
        return success();

    }

}
