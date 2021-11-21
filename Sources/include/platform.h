
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <string>

std::string GetSystemFont();
std::string GetExecutablePath();
bool PathExist(const std::string &path);

#endif
