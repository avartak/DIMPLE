#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <vector>
#include <memory>
#include <string>

namespace dmp {

    struct InputFile;

    struct InputManager {

        std::vector<std::string> filenames;
        std::vector<InputFile*> open;
        std::vector<InputFile*> closed;
        InputFile* currentInputFile;

        InputManager();
        ~InputManager();

        bool isProcessed(const std::string&) const;
        bool isActive(const std::string&) const;
        bool isValid() const;
        std::string getFileName(uint16_t) const;

        bool set(const std::string&);
        bool reset();
    };

}

#endif
