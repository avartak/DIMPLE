#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>

namespace dmp {

    enum Entity_ID {

        ENTITY_TYPE,
        ENTITY_VALUE

    };

    struct Entity {

        uint16_t kind;

        Entity(uint16_t);

        virtual ~Entity() = default;

    };

}

#endif
