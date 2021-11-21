
#pragma once

#include <string>

class Engine
{
public:
    Engine();
    virtual ~Engine();

    void Run();
    void Resize(int width, int height);

    static int GetWidth();
    static int GetHeight();
    static std::string AssetPath(const std::string& path);

protected:
    virtual void OnSize(int width, int height) = 0;
    virtual void Loop(uint64_t delta) = 0;

private:
    void LoadConfig();

private:
    static int s_width;
    static int s_height;

    struct GLFWwindow *m_window = nullptr;
    bool m_fullscreen = false;
    bool m_vsync = true;
};
