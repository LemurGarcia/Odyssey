
#include <fstream>
#include <iostream>
#include <string>
#include <mutex>
#ifdef _WINDOWS
#include <Windows.h>
#endif

static std::ofstream g_file("log.txt");
static std::mutex g_mutex;

void Log(const char *str)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    g_file << str << std::endl;
    g_file.flush();

    std::cout << str << std::endl;
#ifdef _WIN32
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
#endif
}
