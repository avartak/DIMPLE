#include <Scope.h>

namespace avl {

    Scope::Scope() {
    }

    Scope::Scope(const std::shared_ptr<Scope>& p):
        prev(p)
    {
    }

    bool Scope::isDefined(const std::string& n) {

        if (vars.find(n) != vars.end()) {
            return true;
        }
        if (!prev) {
            return false;
        }
        return prev->isDefined(n);

    }

    std::shared_ptr<Variable> Scope::getVar(const std::string& n) {

        std::shared_ptr<Variable> var;
        if (vars.find(n) != vars.end()) {
            return vars[n];
        }
        if (!prev) {
            return var;
        }
        return prev->getVar(n);
    }
}
