
#include <platform.h>

#include <string>
#include <algorithm>
#include <Windows.h>

std::string GetSystemFont()
{
    char file_name[MAX_PATH + 1] = { 0 };
    GetWindowsDirectoryA(file_name, MAX_PATH);
    std::string str_file_name(file_name);
    std::replace(str_file_name.begin(), str_file_name.end(), '\\', '/');
    return str_file_name + "/Fonts/times.ttf";
}

std::string GetExecutablePath()
{
    char file_name[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(nullptr, file_name, MAX_PATH);
    std::string str_file_name(file_name);
    std::replace(str_file_name.begin(), str_file_name.end(), '\\', '/');
    return str_file_name;
}

bool PathExist(const std::string &path)
{
    std::string win_path_wildcard = path + "*";

    WIN32_FIND_DATA ffd = {};
    HANDLE h_find = FindFirstFileA(win_path_wildcard.c_str(), &ffd);
    if (h_find == INVALID_HANDLE_VALUE)
        return false;

    FindClose(h_find);
    return true;
}
