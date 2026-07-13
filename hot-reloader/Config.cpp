#include "HotReloadConfig.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "Util.h"

#undef min

namespace ml::hot_reload {

static constexpr uint32_t DefaultBackupDepth = 3;
static constexpr uint32_t MaxBackupDepth = 10;
static constexpr uint32_t MinBackupDepth = 1;

Config::Config() {
    mBackupDepth = DefaultBackupDepth;
}

bool Config::load(const std::string& fileName) {
    std::filesystem::path configPath = std::filesystem::current_path();

    configPath.append(fileName);

    std::ifstream file = std::ifstream(configPath);
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

        if (key == "emu_path") {
            mEmuPath = std::move(value);
        } else if (key == "mod_path") {
            mModPath = std::move(value);
        } else if (key == "sd_path") {
            mSdPath = std::move(value);
        } else if (key == "backup_depth") {
            mBackupDepth = std::clamp(ml::toUInt32(value, DefaultBackupDepth), MinBackupDepth, MaxBackupDepth);
        }
    }

    return this->isValid();
}

std::string Config::toString() const {
    return std::format("emu={}\nmod={}\nsd={}", mEmuPath, mModPath, mSdPath);
}

bool Config::isValid() const {
    return ml::isExistPath(mEmuPath) && ml::isExistPath(mModPath) && ml::isExistPath(mSdPath);
}

}
