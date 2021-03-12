#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <Location.h>

namespace avl {

    struct Error {
   
        Location location; 
        std::string message;

        Error(const std::string&);
        Error(const Location&, const std::string&);

        std::string print() const;
    };

}

#endif
