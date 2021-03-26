#include <Analyzer.h>

namespace avl {

    Analyzer::Analyzer(const std::shared_ptr<AST>& tree):
        Pass(tree)
    {
    }

    void Analyzer::fail() {
        if (result) {
            result.reset();
        }
    }

    bool Analyzer::run() {

        if (!createTypeReps()) {
            return false;
        }
        if (!createConstReps()) {
            return false;
        }
        if (!createVariables()) {
            return false;
        }
        if (!createFunctions()) {
            return false;
        }
        return true;

    }

    bool Analyzer::createTypeReps() {
        for (const auto& trep : ast->representations) {
            const auto& name = trep.second->name;
            const auto& node = trep.second->node;
            if (types.find(name->name) != types.end()) {
                continue;
            }
            if (!getType(node, true) && hasErrors()) {
                return error(name, "Unable to construct type representation " + name->name);
            }
        }
        return success();
    }

    bool Analyzer::createConstReps() {
        for (const auto& trep : ast->representations) {
            const auto& name = trep.second->name;
            const auto& node = trep.second->node;
            if (constants.find(name->name) != constants.end()) {
                continue;
            }
            if (!getValue(node)) {
                if (hasErrors()) {
                    return error(name, "Unable to construct constant representation " + name->name);
                }
                continue;
            }
            auto constant = std::static_pointer_cast<Value>(result);
            if (!constant->isConst()) {
                return error(name, "Representation " + name->name + " is not a compile-time constant");
            }
            constants[name->name] = constant;
        }
        return success();
    }

    bool Analyzer::createVariables() {

        for (const auto& decl : ast->declarations) {
            const auto& name = decl.second->name;
            if (variables.find(name->name) != variables.end()) {
                continue;
            }
            if (!getGlobalVar(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        for (const auto& defn : ast->definitions) {
            const auto& name = defn.second->name;
            if (variables.find(name->name) != variables.end()) {
                continue;
            }
            if (!getGlobalVar(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        return success();

    }

    bool Analyzer::createFunctions() {

        for (const auto& decl : ast->declarations) {
            const auto& name = decl.second->name;
            if (functions.find(name->name) != functions.end()) {
                continue;
            }
            if (!getFunction(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        for (const auto& defn : ast->definitions) {
            const auto& name = defn.second->name;
            if (functions.find(name->name) != functions.end()) {
                continue;
            }
            if (!getFunction(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        return success();

    }

}
