
#include <platform.h>
#include <cstdint>
#include <vector>
#include <mach-o/dyld.h>
#include <dirent.h>

std::string GetSystemFont()
{
    return "";
}

std::string GetExecutablePath()
{
    uint32_t buffer_size = 512;
    std::vector<char> buffer(buffer_size + 1);

    if (_NSGetExecutablePath(&buffer[0], &buffer_size))
    {
        buffer.resize(buffer_size);
        _NSGetExecutablePath(&buffer[0], &buffer_size);
    }

    std::string s = &buffer[0];
    return s;
}

bool PathExist(const std::string &path)
{
    DIR *dir = opendir(path.c_str());
    if (dir) closedir(dir);
    return !!dir;
}
