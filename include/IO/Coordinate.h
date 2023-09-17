#ifndef COORDINATE_H
#define COORDINATE_H

#include <cstdint>
#include <string>
#include <IO/InputManager.h>

namespace dmp {

    struct Coordinate {

        uint32_t line;
        uint16_t column;

        Coordinate();
        Coordinate(const Coordinate&);
        Coordinate(uint32_t, uint16_t);

    };

}

#endif
