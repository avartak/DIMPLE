#include <InputFile.h>

namespace avl {

    InputFile::InputFile(const InputManager* mgr, uint16_t idx):
        manager(mgr),
        index(idx),
        scanner(nullptr)
    {
        if (mgr != nullptr && mgr->getFileName(idx) != "") {
            file.open(mgr->getFileName(idx), std::ifstream::in);
        }
        if (isOpen()) {
            scanner = new Lexer(file);
        }
    }

    InputFile::~InputFile() {
        if (scanner != nullptr) {
            delete scanner;
        }
    }

    bool InputFile::isOpen() const {
        return file.is_open();
    }

    std::string InputFile::filename() const {
        if (manager) {
            return manager->getFileName(index);
        }
        return "";
    }
}
