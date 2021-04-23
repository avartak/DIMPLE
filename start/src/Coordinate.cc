#include <Coordinate.h>

namespace avl {

    // Default constructor
    // The line and column values are set to 0, and therefore, are invalid
    Coordinate::Coordinate():
        line(0),
        column(0)
    {
    }

    Coordinate::Coordinate(const Coordinate& c):
        line(c.line),
        column(c.column)
    {
    }

    Coordinate::Coordinate(uint32_t l, uint16_t c):
        line(l),
        column(c)
    {
    }

}
