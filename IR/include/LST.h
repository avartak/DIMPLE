#ifndef LST_H
#define LST_H

#include <memory>
#include <map>
#include <string>
#include <Variable.h>

namespace avl {

    struct LST {

        std::shared_ptr<LST> prev;
        std::map<std::string, std::shared_ptr<Variable> > variables;

        LST();
        LST(const std::shared_ptr<LST>&);

        bool isDefined(const std::string&);
        std::shared_ptr<Variable> getVariable(const std::string&);

    };

}

#endif
