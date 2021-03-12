#include <InputFile.h>

namespace avl {

    InputFile::InputFile(const InputManager* mgr, uint16_t idx):
        manager(mgr),
        index(idx)
    {
        if (mgr != nullptr && mgr->getFilePath(idx) != "") {
            file.open(mgr->getFilePath(idx), std::ifstream::in);
        }
        if (isOpen()) {
            scanner = std::make_shared<Lexer>(file);
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
