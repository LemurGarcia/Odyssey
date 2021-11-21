
#include "Engine.h"
#include "VertexBuffer.h"
#include "Shader.h"
#include "Inputs.h"
#include "Font.h"
#include "Json.h"
#include <global.h>
#include <platform.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>

int Engine::s_width = 640;
int Engine::s_height = 480;

#ifdef _DEBUG
constexpr const char *s_engine_config_name = "engine-config-debug.json";
#else
constexpr const char *s_engine_config_name = "engine-config-release.json";
#endif

void WindowSizeFun(GLFWwindow *window, int width, int height)
{
    Engine *app = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    app->Resize(width, height);
}

Engine::Engine()
{
    LoadConfig();

    PV_INFO("GLFW initialisation");
    if (!glfwInit())
    {
        PV_FATAL("Couldn't init GLFW");
    }

    if (s_width == 0 || s_height == 0)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        s_width = mode->width;
        s_height = mode->height;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
#if _GLES
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    PV_INFO("Creating window");
    m_window = glfwCreateWindow(s_width,
                                s_height,
                                "Tony",
                                m_fullscreen ? glfwGetPrimaryMonitor() : nullptr,
                                nullptr);
    PV_ASSERT_MSG(m_window, "Couldn't create a window");

    glfwMakeContextCurrent(m_window);

    PV_INFO("GLEW initialization");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    PV_ASSERT_MSG(err == GLEW_OK, "Couldn't initialize GLEW, error = " << (const char*)glewGetErrorString(err));

    PV_INFO("Renderer: "                  << glGetString(GL_RENDERER));
    PV_INFO("OpenGL version supported "   << glGetString(GL_VERSION));

    glfwSwapInterval(m_vsync);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, WindowSizeFun);

    VertexBuffer::Initialize();
    Program::Initialize();
    Inputs::Initialize(m_window);
    Font::Initialize();

    PV_INFO("Initialization of engine complete");
}

Engine::~Engine()
{
    PV_INFO("Deinitialization of engine");

    Font::Deinitialize();
    Program::Deinitialize();
    VertexBuffer::Deinitialize();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::Run()
{
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(m_window))
    {
        if (Inputs::IsKeyDowned(Inputs::Key::Escape))
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);

        double current_time = glfwGetTime();
        double delta = current_time - last_time;
        last_time = current_time;

        glClear(GL_COLOR_BUFFER_BIT);
        Loop((uint64_t)(1000 * delta));
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Engine::Resize(int width, int height)
{
    if (!width || !height)
        return;

    PV_INFO("Resizing: width = " << width << " height = " << height);
    s_width = width;
    s_height = height;
    glViewport(0, 0, s_width, s_height);
    OnSize(s_width, s_height);
}

int Engine::GetWidth()
{
    return s_width;
}

int Engine::GetHeight()
{
    return s_height;
}

std::string Engine::AssetPath(const std::string& path)
{
    static std::string assets_path;
    if (assets_path.empty())
    {
        std::string path = GetExecutablePath();
        while (!path.empty())
        {
            assets_path = path + "/Assets/";
            if (PathExist(assets_path))
                break;

            path = path.substr(0, path.rfind("/"));
        }
        PV_ASSERT_MSG(!path.empty(), "Coul not find assets folder");
        PV_INFO("Assets folder found in " << assets_path);
    }
    return assets_path + path;
}

void Engine::LoadConfig()
{
    PV_INFO("Loading config " << s_engine_config_name);

    Json config(AssetPath(s_engine_config_name));

    s_width = config.GetValue("Screen", "width", s_width).GetIntValue();
    s_height = config.GetValue("Screen", "height", s_height).GetIntValue();
    m_fullscreen = config.GetValue("Screen", "fullscreen", m_fullscreen).GetBoolValue();
    m_vsync = config.GetValue("Screen", "vsync", m_vsync).GetBoolValue();
}
