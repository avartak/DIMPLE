#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <AST/Node.h>

namespace dmp {

    struct Token : public Node {

        int is;
        std::string str;
        
        Token();
        Token(int, const std::string&, const Location&);

    };

}

#endif
