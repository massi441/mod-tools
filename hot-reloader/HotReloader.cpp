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
    mShutdownEvent = wil::unique_event(wil::EventOptions::ManualReset);
}

bool HotReloader::tryAttachToEmu() {
    std::filesystem::path path = mConfig->emuPath();
    std::wstring emuW = toWString(path.filename().string());

    mAttachedEmuHandle = findProcess(emuW);

    return mAttachedEmuHandle.is_valid();
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
    HANDLE waitHandles[] = { getCurrentHandle(), mShutdownEvent.get() };
    DWORD waitObject = WaitForMultipleObjects(std::size(waitHandles), waitHandles, false, INFINITE );

    if (waitObject == WAIT_FAILED) {
        return false;
    }

    // if the wait was complete because of a shutdown
    if (waitObject == WAIT_OBJECT_0 + 1)  {
        if (!this->closeEmu()) {
            return false;
        }

        if (WaitForSingleObject(this->getCurrentHandle(), INFINITE) == WAIT_FAILED) {
            return false;
        }
    }

    mEmuProcess.reset();
    mAttachedEmuHandle.reset();
    mShutdownEvent.ResetEvent();

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

void HotReloader::requestEmuShutdown() const {
    mShutdownEvent.SetEvent();
}

bool HotReloader::closeEmu() const {
    return TerminateProcess(this->getCurrentHandle(), 1);
}

HANDLE HotReloader::getCurrentHandle() const {
    return mAttachedEmuHandle.is_valid()
       ? mAttachedEmuHandle.get()
       : mEmuProcess.hProcess;
}

}
