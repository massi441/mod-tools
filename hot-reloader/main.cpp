#include <iostream>
#include "HotReloader.h"

static ml::HotReloader hotReloader = ml::HotReloader();
static std::atomic<bool> isRunning = false;

BOOL WINAPI ConsoleCloseHandler(DWORD) {
    if (isRunning) {
        std::cout << "Intercepted shutdown, restoring files from sd folder" << std::endl;
        if (hotReloader.copyModFromSd()) {
            std::cout << "Successfully restored from sd folder, shutting down..." << std::endl;
        } else {
            std::cout << "Failed to restore from sd folder" << std::endl;
        }

        if (hotReloader.closeEmu()) {
            std::cout << "Successfully closed emulator" << std::endl;
        } else {
            std::cerr << "Failed to close emulator during shutdown" << std::endl;
        }

        isRunning = false;
    }

    return FALSE;
}

int main() {
    SetConsoleCtrlHandler(ConsoleCloseHandler, TRUE);

    const ml::Config& config = hotReloader.getConfig();
    if (!config.isValid()) {
        std::cerr << "Failed to initialize config: \n" << config.toString() << std::endl;
        return 1;
    }

    std::cout << "Launching hot reloader for: " << config.emuPath() << std::endl;

    if (!hotReloader.tryAttachToEmu() && !hotReloader.launchEmu()) {
        std::cout << "Launched emulator successfully" << std::endl;
        return 1;
    }

    std::cout << "Hot reloader initialized, backing up mod and copying files from sd card..." << std::endl;

    if (!hotReloader.backupMod()) {
        std::cout << "Failed to backup mod from sd folder" << std::endl;
        return 1;
    }

    if (!hotReloader.copyModFromSd()) {
        std::cerr << "Failed to copy files from sd folder" << std::endl;
        return 1;
    }

    isRunning = true;

    std::cout << "Copied files from sd card, waiting for emulator to be shutdown..." << std::endl;

    if (!hotReloader.waitEmuExit()) {
        std::cout << "Failed to wait for emulator shutdown" << std::endl;
        return 1;
    }

    std::cout << "Emulator shutdown successfully, restoring files from sd card" << std::endl;

    if (!hotReloader.copyModFromSd()) {
        std::cerr << "Failed to restore from sd folder" << std::endl;
        return 1;
    }

    isRunning = false;

    std::string line;
    std::getline(std::cin, line);

    return 0;
}
