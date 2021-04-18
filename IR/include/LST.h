#ifndef LST_H
#define LST_H

#include <memory>
#include <map>
#include <string>
#include <Variable.h>
#include <Function.h>

namespace avl {

    struct LST {

        std::shared_ptr<LST> prev;
        std::map<std::string, std::shared_ptr<Variable> > variables;
        std::map<std::string, std::shared_ptr<Function> > functions;

        LST();
        LST(const Function&);

        bool isDefined(const std::string&);
        bool isDefinedInThisScope(const std::string&);
        std::shared_ptr<Variable> getVariable(const std::string&);
        std::shared_ptr<Function> getFunction(const std::string&);
        std::shared_ptr<Instance> getInstance(const std::string&);

    };

}

#endif
