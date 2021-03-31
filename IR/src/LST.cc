#include <LST.h>

namespace avl {

    LST::LST() {
    }

    LST::LST(const std::shared_ptr<LST>& p):
        prev(p)
    {
    }

    bool LST::isDefined(const std::string& n) {

        if (variables.find(n) != variables.end()) {
            return true;
        }
        if (!prev) {
            return false;
        }
        return prev->isDefined(n);

    }

    std::shared_ptr<Variable> LST::getVariable(const std::string& n) {

        std::shared_ptr<Variable> var;
        if (variables.find(n) != variables.end()) {
            return variables[n];
        }
        if (!prev) {
            return var;
        }
        return prev->getVariable(n);
    }
}
