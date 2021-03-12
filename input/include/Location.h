/*

Location struct represents the location of a Node or a syntactic unit
in a source file. It carries the following information 
- A file index (file_index : uint16_t) that can be used to obtain 
  the file name from the InputManager
- The start and end Coordinates (line, column) of the Node 

*/

#ifndef LOCATION_H
#define LOCATION_H

#include <cstdint>
#include <string>
#include <Coordinate.h>

namespace avl {

    struct Location {

        Coordinate start;
        Coordinate end;
        uint16_t file_index;

        Location();
        Location(const Location&);
        Location(uint16_t, const Coordinate&);
        Location(uint16_t, const Coordinate&, const Coordinate&);

        std::string filename() const;

    };

}

#endif
