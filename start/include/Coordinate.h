/*

Coordinate struct represents a line and column number in a source file
Valid line and column numbers start from 1
- line is a 32-bit unsigned integer
- column is a 16-bit unsigned integer

*/

#ifndef COORDINATE_H
#define COORDINATE_H

#include <cstdint>

namespace avl {

    struct Coordinate {

        uint32_t line;
        uint16_t column;

        Coordinate();
        Coordinate(const Coordinate&);
        Coordinate(uint32_t, uint16_t);

    };

}

#endif
