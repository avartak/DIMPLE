#include <Analyzer.h>

namespace avl {

    Analyzer::Analyzer(const std::shared_ptr<AST>& tree, const std::shared_ptr<GST>& sym):
        Pass(nullptr, tree, sym)
    {
    }

    void Analyzer::fail() {
        result.reset();
    }

    bool Analyzer::run() {
        return createRepresentations() && createGlobals();
    }

    bool Analyzer::createRepresentations() {
        for (const auto& rep : ast->representations) {
            const auto& name = rep.second->name;
            const auto& node = rep.second->node;

            auto nsnode = ast->getNonSynonymRepNode(name);
            if (nsnode->kind == NODE_IDENTIFIER) {
                auto ident = static_cast<Identifier*>(nsnode.get());
                return error(nsnode, ident->name + " is not a representation");
            }
            if (getTypeRep(name) || getConstRep(name)) {
                continue;
            }
            return error(name, "Unable to construct representation " + name->name);
        }
        return success();
    }

    bool Analyzer::createGlobals() {
        for (const auto& decl : ast->declarations) {
            const auto& name = decl.second->name;
            if (!getGlobalInstance(name)) {
                return error(name, "Unable to construct global " + name->name);
            }
        }
        for (const auto& defn : ast->definitions) {
            const auto& name = defn.second->name;
            if (!getGlobalInstance(name)) {
                return error(name, "Unable to construct global " + name->name);
            }
        }
        return success();
    }

}
