#ifndef LOCATION_H
#define LOCATION_H

#include <cstdint>
#include <string>
#include <IO/Coordinate.h>
#include <IO/InputManager.h>

namespace dmp {

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
