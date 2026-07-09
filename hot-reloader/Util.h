#pragma once

#include <filesystem>
#include <string>
#include <wil/resource.h>

namespace ml {

wil::unique_handle findProcess(const std::wstring& processName, DWORD access = PROCESS_ALL_ACCESS);
std::wstring toWString(const std::string& str);
uint32_t toUInt32(const std::string& str, uint32_t fallback);
bool backupDir(const std::filesystem::path& path, const std::filesystem::path& backupPath, std::filesystem::copy_options options);
bool ensureDirCreated(const std::filesystem::path& path);
bool clearDirectory(const std::filesystem::path& path);
bool isExistPath(const std::filesystem::path& path);

}
