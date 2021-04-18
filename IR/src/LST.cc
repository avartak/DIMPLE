#include <LST.h>

namespace avl {

    LST::LST() {
    }

    LST::LST(const Function& f):
        prev(f.lst)
    {
    }

    bool LST::isDefined(const std::string& n) {

        if (variables.find(n) != variables.end()) {
            return true;
        }
        if (functions.find(n) != functions.end()) {
            return true;
        }
        if (!prev) {
            return false;
        }
        return prev->isDefined(n);

    }

    bool LST::isDefinedInThisScope(const std::string& n) {

        if (variables.find(n) != variables.end()) {
            return true;
        }
        if (functions.find(n) != functions.end()) {
            return true;
        }
        return false;

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

    std::shared_ptr<Function> LST::getFunction(const std::string& n) {

        std::shared_ptr<Function> var;
        if (functions.find(n) != functions.end()) {
            return functions[n];
        }
        if (!prev) {
            return var;
        }
        return prev->getFunction(n);
    }

    std::shared_ptr<Instance> LST::getInstance(const std::string& n) {

        std::shared_ptr<Instance> inst;
        if (functions.find(n) != functions.end()) {
            return functions[n];
        }
        if (variables.find(n) != variables.end()) {
            return variables[n];
        }
        if (!prev) {
            return inst;
        }
        return prev->getInstance(n);
    }

}
