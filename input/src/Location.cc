#include <Location.h>
#include <Globals.h>

namespace avl {

    Location::Location():
        file_index(-1),
        start(),
        end()
    {
    }

    Location::Location(const Location& l):
        file_index(l.file_index),
        start(l.start),
        end(l.end)
    {
    }

    Location::Location(uint16_t f, const Coordinate& s):
        file_index(f),
        start(s)
    {
    }

    Location::Location(uint16_t f, const Coordinate& s, const Coordinate& e):
        file_index(f),
        start(s),
        end(e)
    {
    }

    std::string Location::filename() const {
        return TheInput->getFileName(file_index);
    }

}
