#include "Util.h"

#include <TlHelp32.h>
#include <utf8cpp/utf8.h>

namespace ml::hot_reload {

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
                break;
            }
        } while (Process32NextW(snapShot, &entry));
    }

    CloseHandle(snapShot);

    if (pid == -1) {
        return wil::unique_handle();
    }

    return wil::unique_handle(OpenProcess(access, FALSE, pid));
}

uint32_t findProcessCount(const std::wstring &processName) {
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapShot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    uint32_t count = 0;
    PROCESSENTRY32W entry{ sizeof(entry) };
    if (Process32FirstW(snapShot, &entry)) {
        do {
            if (processName == entry.szExeFile) {
                count++;
            }
        } while (Process32NextW(snapShot, &entry));
    }

    CloseHandle(snapShot);

    return count;
}

std::wstring toWString(const std::string& str) {
    std::wstring ws;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(ws));
    return ws;
}

}
