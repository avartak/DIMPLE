#include <sstream>
#include <Common/Error.h>

namespace dmp {

    Error::Error(const std::string& m):
        location(),
        message(m)
    {
    }

    Error::Error(const Location& l, const std::string& m):
        location(l),
        message(m)
    {
    }

    std::string Error::print(const InputManager* in) const {

        std::stringstream out;
        out << "[DIMPLE error]";
        if (location.filename(in) != "") {
            out << " " << location.filename(in);
            if (location.start.line > 0) {
                out << ":" << location.start.line << ":" << location.start.column;
                if (location.end.line > location.start.line) {
                    out << "-" << location.start.line << ":" << location.end.column;
                }
                else if (location.end.line == location.start.line && 
                         location.end.column > location.start.column) 
                {
                    out << "-" << location.end.column;
                }
            }
        }
        out << " " << message << std::endl;

        return out.str();
    }

}
