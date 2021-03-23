#ifndef SCOPE_H
#define SCOPE_H

#include <memory>
#include <map>
#include <string>
#include <Variable.h>

namespace avl {

    struct Scope {

        std::shared_ptr<Scope> prev;
        std::map<std::string, std::shared_ptr<Variable> > vars;

        Scope();
        Scope(const std::shared_ptr<Scope>&);

        bool isDefined(const std::string&);
        std::shared_ptr<Variable> getVar(const std::string&);

    };

}

#endif
