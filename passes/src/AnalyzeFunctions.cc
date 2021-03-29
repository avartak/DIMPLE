#include <llvm/IR/Value.h>
#include <Analyzer.h>
#include <FunctionType.h>
#include <VoidType.h>
#include <BinaryOp.h>
#include <FunctionOp.h>
#include <Statement.h>
#include <BlockNode.h>
#include <Globals.h>

namespace avl {

    bool Analyzer::getFunction(const std::shared_ptr<Identifier>& ident) {

        const auto& n = ident->name;

        if (variables.find(n) != variables.end()) {
            if (!variables.find(n)->second) {
                return error("Unable to completely define variable " + n);
            }

            result = variables[n];
            return success();
        }

        if (ast->declarations.find(n) == ast->declarations.end() &&
            ast->definitions.find(n) == ast->definitions.end())
        {
            return error();
        }

        std::shared_ptr<Variable> var;
        std::shared_ptr<Node> tnode;
        std::shared_ptr<Node> defn;
        uint16_t storage = STORAGE_EXTERNAL;

        if (ast->declarations.find(n) != ast->declarations.end()) {
            tnode = ast->declarations[n]->node;
        }
        else {
            tnode = ast->definitions[n]->type;
            storage = ast->definitions[n]->storage;
        }

        if (!getType(tnode, false)) {
            return error(ident, "Unable to determine type of " + n);
        }
        auto type = std::static_pointer_cast<Type>(result);
        if (!type->isFunction()) {
            return error();
        }
        if (!type->isComplete()) {
            return error(ident, "Type of " + n + " is not completely defined");
        }

        if (n == "start") {
            auto ftype = static_cast<FunctionType*>(type.get());
            bool sigisgood = true;
            if (ftype->ret->is != TYPE_INT32) {
                sigisgood = false;
            }
            if (ftype->args.size() != 2) {
                sigisgood = false;
            }
            if (ftype->args[0].type->is != TYPE_INT32) {
                sigisgood = false;
            } 
            if (!ftype->args[0].type->isPtr()) {
                sigisgood = false;
            }
            auto pt = static_cast<PointerType*>(ftype->args[0].type.get())->points_to;
            if (!pt->isPtr()) {
                sigisgood = false;
            }
            auto ppt = static_cast<PointerType*>(pt.get())->points_to;
            if (ppt->is != TYPE_INT8) {
                sigisgood = false;
            }
            if (!sigisgood) {
                return error(ident, "Incorrect signature of \'start\' function");
            }
        }

        if (ast->definitions.find(n) != ast->definitions.end()) {
            storage = ast->definitions[n]->storage;
            functions[n] = std::make_shared<Function>(storage, n, type);
            currentFunction = functions[n];
            currentFunction->init();
            if (!defineCurrentFunction(ast->definitions[n])) {
                return error(ast->definitions[n]->name, "Unable to define function " + n);
            }
            currentFunction.reset();
        }
        else {
            functions[n] = std::make_shared<Function>(storage, n, type);
        }

        result = functions[n];
        return success();
    }

    bool Analyzer::defineCurrentFunction(const std::shared_ptr<DefineStatement>& defn) {
        
        auto fnode = static_cast<Node*>(defn->def.get());

        if (fnode->kind == NODE_NULLINIT) {
            auto ft = static_cast<FunctionType*>(currentFunction->type.get());
            if (!ft->ret->isVoid()) {
                return error(defn->name, "Function with a non-void return type cannot have an empty body");
            }
            TheBuilder.CreateRetVoid();
            return success();
        }
        else {
            auto funcblock = static_cast<FuncBlockNode*>(fnode);
            for (const auto& statement : funcblock->body) {
                if (statement->is == STATEMENT_BREAK) {
                    return error(statement, "\'break\' statement is not allowed in this block");
                }
                else if (statement->is == STATEMENT_CONTINUE) {
                    return error(statement, "\'continue\' statement is not allowed in this block");
                }
            }

            if (!defineBlock(funcblock->body)) {
                return error(defn->name, "Unable to define \'" + defn->name->name + "\'");
            }
            if (!currentFunction->checkTerminations()) {
                return error(defn->name, "Function \'" + defn->name->name + "\' does not always return");
            }
            return success();
        }

    }

    bool Analyzer::call(const std::shared_ptr<CallExprNode>& callex, const std::shared_ptr<Variable>& retv) {

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
        for (const auto& arg : args) {
            if (!getValue(arg)) {
                return error(arg, "Unable to construct function argument");
            }
            argvals.push_back(std::static_pointer_cast<Value>(result));
        }
        for (std::size_t i = 0; i < ft->args.size(); i++) {
            if (ft->args[i].type->isPtr() && argvals[i]->type->isPtr()) {
                auto fpt = static_cast<PointerType*>(ft->args[i].type.get());
                auto apt = static_cast<PointerType*>(argvals[i]->type.get());
                if (fpt->points_to->isUnknown() || apt->points_to->isUnknown()) {
                    argvals[i] = BinaryOp::recast(argvals[i], ft->args[i].type);
                }
            }
            if (*ft->args[i].type != *argvals[i]->type) {
                return error(args[i], "Function argument " + std::to_string(i+1) + " has inconsistent type");
            }
        }

        result = bool(retv) ? FunctionOp::call(func, argvals, retv) : FunctionOp::call(func, argvals);
        if (!result) {
            return error(callex, "Function call failed"); // This should never happen
        }
        return success();
    }

    bool Analyzer::defineBlock(const std::vector<std::shared_ptr<Statement> >&, std::shared_ptr<CodeBlock>, std::shared_ptr<CodeBlock>) {

        return error();

    }
}
