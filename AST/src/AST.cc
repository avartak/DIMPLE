#include <AST.h>

namespace avl {

    std::shared_ptr<Node> AST::getNonSynonymRepNode(const std::shared_ptr<Identifier>& ident) {

        if (representations.find(ident->name) == representations.end()) {
            return ident;
        }
        auto next_node = representations[ident->name]->node;
        while (next_node->kind == NODE_IDENTIFIER) {
            auto next_ident = std::static_pointer_cast<Identifier>(next_node);
            const auto& nm = next_ident->name;
            if (representations.find(nm) == representations.end()) {
                break;
            }
            next_node = representations[nm]->node;
        }
        return next_node;
    }

}
