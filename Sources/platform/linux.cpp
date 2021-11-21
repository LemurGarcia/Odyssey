
#include <platform.h>
#include <cstdint>
#include <vector>

#include <dirent.h>
#include <libgen.h>

std::string GetSystemFont()
{
    return "";
}

std::string GetExecutablePath()
{
   return "/home/pi/projects/ploskiyvision/_build/Sources/PloskyVision";
}

bool PathExist(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir) closedir(dir);
    return !!dir;
}
