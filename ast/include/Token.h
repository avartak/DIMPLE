#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <Node.h>

namespace avl {

    struct Token : public Node {

        int is;
        std::string str;
        
        Token();
        Token(int, const std::string&, const Location&);

    };

}

#endif
