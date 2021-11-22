#pragma once
#include <string>

struct Point
{
    int x = 0;
    int y = 0;
};

class Level
{
private:
    std::wstring m_borders =
        LR"(
zzzzzzzzzzzzzzzzzzzzzzzzzzz
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
z_________________________z
zzzzzzzzzzzzzzzzzzzzzzzzzzz
)";
    Point m_meal;
public:
    Level();
    Point GetSize();
    std::wstring GetStringByLine(int line);
};