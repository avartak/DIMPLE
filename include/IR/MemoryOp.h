#ifndef MEMORYOP_H
#define MEMORYOP_H

#include <IR/Variable.h>

namespace dmp {

    struct MemoryOp {

        static bool memset(Variable*, uint8_t);
        static bool memset(const std::shared_ptr<Variable>&, uint8_t);
        static bool memcpy(const std::shared_ptr<Variable>&, const std::shared_ptr<Variable>&);

    };

}

#endif
