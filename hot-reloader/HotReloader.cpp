#include "HotReloader.h"

#include <filesystem>
#include "Util.h"

namespace ml {

static constexpr std::filesystem::copy_options sCopyOptions =
    std::filesystem::copy_options::recursive |
    std::filesystem::copy_options::overwrite_existing;

HotReloader::HotReloader() {
    mConfig = std::make_unique<Config>();
    mConfig->load();
}

bool HotReloader::tryAttachToEmu() {
    std::filesystem::path path = mConfig->emuPath();
    std::wstring emuW = toWString(path.filename().string());

    mRunningEmuHandle = findProcess(emuW);

    return mRunningEmuHandle.is_valid();
}

bool HotReloader::launchEmu() {
    STARTUPINFOA startup_info = { .cb = sizeof(STARTUPINFOA) };

    return CreateProcessA(
        mConfig->emuPath().c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NEW_CONSOLE,
        nullptr,
        nullptr,
        &startup_info,
        &mEmuProcess
    );
}

bool HotReloader::waitEmuExit() {
    WaitForSingleObject(this->getCurrentHandle(), INFINITE);

    mEmuProcess.reset();
    mRunningEmuHandle.reset();

    return true;
}

bool HotReloader::backupMod() const {
    std::filesystem::path backupPath = mConfig->modPath();
    backupPath.replace_filename("backup");

    return backupDir(mConfig->modPath(), backupPath, sCopyOptions);
}

bool HotReloader::copyModFromSd() const {
    if (!clearDirectory(mConfig->modPath())) {
        return false;
    }

    std::error_code ec;
    std::filesystem::copy(mConfig->sdPath(), mConfig->modPath(), sCopyOptions, ec);

    return !ec;
}

bool HotReloader::closeEmu() const {
    return TerminateProcess(this->getCurrentHandle(), 1);
}

HANDLE HotReloader::getCurrentHandle() const {
    return mRunningEmuHandle.is_valid()
       ? mRunningEmuHandle.get()
       : mEmuProcess.hProcess;
}

}
