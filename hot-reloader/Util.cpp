#include "Util.h"

#include <TlHelp32.h>
#include <utf8cpp/utf8.h>
#include <vector>

namespace ml {

wil::unique_handle findProcess(const std::wstring& processName, DWORD access) {
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapShot == INVALID_HANDLE_VALUE) {
        return wil::unique_handle();
    }

    DWORD pid = -1;
    PROCESSENTRY32W entry{ sizeof(entry) };

    if (Process32FirstW(snapShot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                pid = entry.th32ProcessID;
            }
        } while (Process32NextW(snapShot, &entry));
    }

    CloseHandle(snapShot);

    if (pid == -1) {
        return wil::unique_handle();
    }

    return wil::unique_handle(OpenProcess(access, FALSE, pid));
}

std::wstring toWString(const std::string& str) {
    std::wstring ws;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(ws));
    return ws;
}

bool backupDir(const std::filesystem::path& path, const std::filesystem::path& backupPath, std::filesystem::copy_options options) {
    if (isExistPath(backupPath)) {
        if (!clearDirectory(backupPath)) {
            return false;
        }
    } else {
        std::filesystem::create_directory(backupPath);
    }

    std::error_code ec;
    std::filesystem::path backupCan = std::filesystem::weakly_canonical(backupPath, ec);

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) {
            return false;
        }

        std::filesystem::path entryPathCan = std::filesystem::weakly_canonical(entry.path(), ec);
        if (entryPathCan == backupCan) {
            continue; // skip backing up the back up directory
        }

        std::filesystem::copy(entry.path(), backupPath / entry.path().filename(), options, ec);
        if (ec) {
            return false;
        }
    }

    return !ec;
}

bool ensureDirCreated(const std::filesystem::path &path) {
    std::error_code ec;
    if (!std::filesystem::exists(path, ec)) {
        std::filesystem::create_directory(path, ec);

        if (ec) {
            return false;
        }
    }

    return true;
}

bool clearDirectory(const std::filesystem::path &path) {
    std::error_code ec;

    std::vector<std::filesystem::path> removeableEntries;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path, ec)) {
        if (ec) {
            return false;
        }

        removeableEntries.push_back(entry.path());
    }

    for (const std::filesystem::path& removeableEntry : removeableEntries) {
        std::filesystem::remove_all(removeableEntry, ec);
        if (ec) {
            return false;
        }
    }

    return !ec;
}

bool isExistPath(const std::filesystem::path &path) {
    std::error_code ec;
    return std::filesystem::exists(path, ec);
}

}
