#pragma once

#include <string>
#include <vector>

namespace ml::hot_reload {

class Config {
public:
    Config();

    bool load(const std::string& fileName = "config.txt");

    const std::string& emuPath() const { return mEmuPath; }
    const std::string& modPath() const { return mModPath; }
    const std::string& sdPath() const { return mSdPath; }
    uint32_t backupDepth() const { return mBackupDepth; }

    std::string toString() const;

    bool isValid() const;

private:
    std::string mEmuPath;
    std::string mModPath;
    std::string mSdPath;
    uint32_t mBackupDepth;
};

}
