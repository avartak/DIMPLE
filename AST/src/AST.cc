#include <AST.h>

namespace dmp {

    std::shared_ptr<Node> AST::getNonSynonymRepNode(const std::shared_ptr<Identifier>& ident) {

        if (representations.find(ident->name) == representations.end()) {
            return ident;
        }
        auto next_node = representations[ident->name]->node;
        while (next_node->kind == NODE_IDENTIFIER) {
            const auto& nm = static_cast<Identifier*>(next_node.get())->name;
            if (representations.find(nm) == representations.end()) {
                break;
            }
            next_node = representations[nm]->node;
        }
        return next_node;
    }

}
