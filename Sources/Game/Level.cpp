#include "Level.h"

Level::Level()
{
}

Point Level::GetSize()
{
    return Point{ 18,28 };
}

std::wstring Level::GetStringByLine(int line)
{
    std::wstring curr_string;
    for (int i = 0; i < 28; i++)
    {
        curr_string += m_borders[i + 28*line];
    }
    return curr_string;
}