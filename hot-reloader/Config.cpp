#include "Config.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Util.h"

namespace ml {

bool Config::load(const std::string& fileName) {
    std::filesystem::path currentPath = std::filesystem::current_path();

    currentPath.append(fileName);

    std::ifstream file = std::ifstream(currentPath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t equalSign = line.find('=');
        if (equalSign == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, equalSign);
        std::string value = line.substr(equalSign + 1);

        if (key == "emuPath") {
            mEmuPath = std::move(value);
        } else if (key == "modPath") {
            mModPath = std::move(value);
        } else if (key == "sdPath") {
            mSdPath = std::move(value);
        }
    }

    return this->isValid();
}

std::string Config::toString() const {
    return std::format("emu={}\nmod={}\nsd={}", mEmuPath, mModPath, mSdPath);
}

bool Config::isValid() const {
    return isExistPath(mEmuPath) && isExistPath(mModPath) && isExistPath(mSdPath);
}

}
