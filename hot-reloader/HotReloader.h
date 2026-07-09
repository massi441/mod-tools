#pragma once

#include <memory>
#include <wil/resource.h>
#include "Config.h"

namespace ml {

class HotReloader {
public:
    explicit HotReloader();

    bool tryAttachToEmu();
    bool launchEmu();
    bool waitEmuExit();
    bool backupMod() const;
    bool copyModFromSd() const;

    void requestEmuShutdown() const;

    const Config& getConfig() const { return *mConfig; }

private:
    std::unique_ptr<Config> mConfig;
    wil::unique_handle mAttachedEmuHandle; // emu handle if it's already running
    wil::unique_process_information mEmuProcess; // emu process info if created by the hot reloader
    wil::unique_event mShutdownEvent;

    HANDLE getCurrentHandle() const;
    bool closeEmu() const;
};

}
