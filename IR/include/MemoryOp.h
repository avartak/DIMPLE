#ifndef MEMORYOP_H
#define MEMORYOP_H

#include <Variable.h>

namespace avl {

    struct MemoryOp {

        static bool memset(const std::shared_ptr<Variable>&, uint8_t);
        static bool memcpy(const std::shared_ptr<Variable>&, const std::shared_ptr<Variable>&);

    };

}

#endif
