
#include "Engine/Engine.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/Inputs.h"
#include "Engine/Font.h"
#include <global.h>

static int CalcFPS(uint64_t delta)
{
    static int fps = 0;
    static int frames = 0;
    static uint64_t time = 0;

    frames++;
    time += delta;

    if (time > 1000)
    {
        fps = frames;
        frames = 0;
        time -= 1000;
    }
    return fps;
}

class Application : public Engine
{
public:
    Application() : Engine()
    {
        OnSize(Engine::GetWidth(), Engine::GetHeight());
        PV_INFO("Load game complete");
    }

    ~Application() override
    {
        PV_INFO("Deinitialization of game");
    }

protected:
    void OnSize(int width, int height) override
    {
        Font::Resize(height / 32.f);
    }

    void Loop(uint64_t delta) override
    {
        Font::Render(L"#gFPS = " + std::to_wstring(CalcFPS(delta)), { -0.95f, -0.85f });
        Font::Render(L"#WHe-hey! #W", { -0.95f, -0.75f });
    }
};

int main(int argc, const char* argv[])
{
    Application app;
    app.Run();
    return 0;
}