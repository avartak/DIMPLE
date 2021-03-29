#include <Analyzer.h>

namespace avl {

    bool Analyzer::getFunction(const std::shared_ptr<Identifier>&) {

        return error();

    }

    bool Analyzer::call(const std::shared_ptr<CallExprNode>&) {

        return error();

    }

    bool Analyzer::call(const std::shared_ptr<CallExprNode>&, const std::shared_ptr<Variable>&) {

        return error();

    }
}

