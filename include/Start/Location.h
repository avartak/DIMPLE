#ifndef LOCATION_H
#define LOCATION_H

#include <cstdint>
#include <string>
#include <Start/InputManager.h>

namespace dmp {

    struct Coordinate {

        uint32_t line;
        uint16_t column;

        Coordinate();
        Coordinate(const Coordinate&);
        Coordinate(uint32_t, uint16_t);

    };

    struct Location {

        Coordinate start;
        Coordinate end;
        uint16_t file_index;

        Location();
        Location(const Location&);
        Location(uint16_t, const Coordinate&);
        Location(uint16_t, const Coordinate&, const Coordinate&);

        std::string filename(const InputManager*) const;

    };

}

#endif
