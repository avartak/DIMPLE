#include <InputManager.h>
#include <InputFile.h>

namespace avl {

    InputManager::InputManager():
        currentInputFile(nullptr)
    {
    }

    InputManager::~InputManager() {
        for (auto iopen : open) {
            delete iopen;
        }
        for (auto iclosed : closed) {
            delete iclosed;
        }
    }

    bool InputManager::isActive(const std::string& filename) const {
        for (const auto& file : open) {
            if (file->filename() == filename) {
                return true;
            }
        }
        return false;
    }

    bool InputManager::isProcessed(const std::string& filename) const {
        for (const auto& file : closed) {
            if (file->filename() == filename) {
                return true;
            }
        }
        return false;
    }

    bool InputManager::isValid() const {
        return (currentInputFile != nullptr) && currentInputFile->isOpen();
    }

    std::string InputManager::getFileName(uint16_t idx) const {
        if (idx < filenames.size()) {
            return filenames[idx];
        }
        return "";
    }

    bool InputManager::set(const std::string& filename) {
        if (isActive(filename) || isProcessed(filename)) {
            return false;
        }
        uint16_t idx = filenames.size();
        filenames.push_back(filename);
        open.push_back(new InputFile(this, idx));
        currentInputFile = open.back();
        return true;
    }

    bool InputManager::reset() {
        if (open.size() == 0) {
            currentInputFile = nullptr;
            return false;
        }

        closed.push_back(open.back());
        open.pop_back();
        if (open.size() > 0) {
            currentInputFile = open.back();
        }
        else {
            currentInputFile = nullptr;
        }
        return true;
    }
}
