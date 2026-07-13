#pragma once

#include <filesystem>
#include <string>
#include <wil/resource.h>

namespace ml::hot_reload {

// TODO: Abstract host os (and avoid using wstring as param)

wil::unique_handle findProcess(const std::wstring& processName, DWORD access = PROCESS_ALL_ACCESS);
uint32_t findProcessCount(const std::wstring& processName);
std::wstring toWString(const std::string& str);

}
