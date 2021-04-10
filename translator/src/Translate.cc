#include <Translator.h>

namespace avl {

    Translator::Translator(InputManager* in, AST* tree, GST* sym):
        Pass(in, tree, sym)
    {
    }

    void Translator::fail() {
        result.reset();
    }

    bool Translator::run() {
        return createRepresentations() && createGlobals();
    }

    bool Translator::createRepresentations() {
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

    bool Translator::createGlobals() {
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
