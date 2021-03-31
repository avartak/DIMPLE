#include <Analyzer.h>

namespace avl {

    Analyzer::Analyzer(const std::shared_ptr<AST>& tree, const std::shared_ptr<GST>& sym):
        Pass(nullptr, tree, sym)
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
            if (gst->types.find(name->name) != gst->types.end()) {
                continue;
            }
            if (!getType(node, false) && hasErrors()) {
                return error(name, "Unable to construct type representation " + name->name);
            }
        }
        return success();
    }

    bool Analyzer::createConstReps() {
        for (const auto& trep : ast->representations) {
            const auto& name = trep.second->name;
            const auto& node = trep.second->node;
            if (gst->constants.find(name->name) != gst->constants.end()) {
                continue;
            }
            if (!getValue(node) && hasErrors()) {
                return error(name, "Unable to construct constant representation " + name->name);
            }
        }
        return success();
    }

    bool Analyzer::createVariables() {

        for (const auto& decl : ast->declarations) {
            const auto& name = decl.second->name;
            if (gst->variables.find(name->name) != gst->variables.end()) {
                continue;
            }
            if (!getGlobalVar(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        for (const auto& defn : ast->definitions) {
            const auto& name = defn.second->name;
            if (gst->variables.find(name->name) != gst->variables.end()) {
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
            if (gst->functions.find(name->name) != gst->functions.end()) {
                continue;
            }
            if (!getFunction(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        for (const auto& defn : ast->definitions) {
            const auto& name = defn.second->name;
            if (gst->functions.find(name->name) != gst->functions.end()) {
                continue;
            }
            if (!getFunction(name) && hasErrors()) {
                return error(name, "Unable to construct global " + name->name);
            }
        }

        return success();

    }

}
