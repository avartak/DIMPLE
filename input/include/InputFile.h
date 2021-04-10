#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <string>
#include <fstream>
#include <Lexer.h>
#include <InputManager.h>

namespace avl {

    struct InputFile {

        const InputManager* manager;
        uint16_t index;
        std::ifstream file;
        Lexer* scanner;

        InputFile(const InputManager*, uint16_t);
        ~InputFile();

        bool isOpen() const;
        std::string filename() const;

    };

}

#endif
