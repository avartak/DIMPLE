#include <InputManager.h>
#include <InputFile.h>

namespace avl {

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
        return currentInputFile && currentInputFile->isOpen();
    }

    std::string InputManager::getFileName(uint16_t idx) const {
        if (idx < filenames.size()) {
            return filenames[idx];
        }
        return "";
    }

    std::string InputManager::getFilePath(uint16_t idx) const {
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
        auto parsefile = std::make_shared<InputFile>(this, idx);
        open.push_back(parsefile);
        currentInputFile = parsefile;
        return true;
    }

    bool InputManager::reset() {
        if (open.size() == 0) {
            currentInputFile.reset();
            return false;
        }

        closed.push_back(open.back());
        open.pop_back();
        if (open.size() > 0) {
            currentInputFile = open.back();
        }
        else {
            currentInputFile.reset();
        }
        return true;
    }
}
