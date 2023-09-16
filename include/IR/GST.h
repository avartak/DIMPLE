#ifndef GST_H
#define GST_H

#include <map>
#include <string>
#include <memory>

#include <IR/Type.h>
#include <IR/Value.h>
#include <IR/Variable.h>
#include <IR/Function.h>

namespace dmp {

    struct GST {
        std::map<std::string, std::shared_ptr<Type> > types;
        std::map<std::string, std::shared_ptr<Value> > constants;
        std::map<std::string, std::shared_ptr<Variable> > variables;
        std::map<std::string, std::shared_ptr<Function> > functions;
    };

}

#endif
