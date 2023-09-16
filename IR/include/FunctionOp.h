#ifndef FUNCTIONOP_H
#define FUNCTIONOP_H

#include <memory>
#include <Function.h>
#include <Variable.h>

namespace dmp {

    struct FunctionOp {

        static std::shared_ptr<Value> call(const std::shared_ptr<Function>&, const std::vector<std::shared_ptr<Value> >&);
        static std::shared_ptr<Value> call(const std::shared_ptr<Function>&, const std::vector<std::shared_ptr<Value> >&, const std::shared_ptr<Variable>&);
        static bool ret(const std::shared_ptr<Function>&, const std::shared_ptr<Value>&);

    };

}

#endif
