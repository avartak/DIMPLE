#ifndef LITERAL_H
#define LITERAL_H

#include <memory>
#include <string>
#include <Value.h>

namespace dmp {

    struct IntLiteral : public Value {

        uint64_t literal;

        IntLiteral(uint64_t);

    };

    struct BoolLiteral : public Value {

        bool literal;

        BoolLiteral(bool);

    };

    struct RealLiteral : public Value {

        double literal;

        RealLiteral(double);

    };

    struct CharLiteral : public Value {

        char literal;

        CharLiteral(char);

    };

    struct StringLiteral : public Value {

        std::string literal;

        StringLiteral(const std::string&);

    };

}

#endif
