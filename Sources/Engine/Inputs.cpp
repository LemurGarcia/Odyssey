
#include "Inputs.h"
#include <global.h>
#include <Engine/Engine.h>
#include <GLFW/glfw3.h>
#include <map>
#include <array>

static GLFWwindow* s_window = nullptr;
static std::map<int, Inputs::Key> s_key_bindings;
static std::array<bool, static_cast<uint32_t>(Inputs::Key::Count)> s_keys_downed;
static std::array<bool, static_cast<uint32_t>(Inputs::Key::Count)> s_keys_pressed;

static bool s_mouse_downed = false;
static bool s_mouse_pressed = false;

static void UpdateKey(Inputs::Key key, int action)
{
    uint32_t key_type = static_cast<uint32_t>(key);

    if (action == GLFW_PRESS)
    {
        s_keys_pressed[key_type] = !s_keys_downed[key_type];
        s_keys_downed[key_type] = true;
    }
    else if (action == GLFW_RELEASE)
        s_keys_downed[key_type] = false;
}

static void KeyFun(GLFWwindow*, int key, int, int action, int mod)
{
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    auto found = s_key_bindings.find(key);
    if (found == s_key_bindings.end())
        return;

    UpdateKey(found->second, action);
}

void Inputs::Initialize(GLFWwindow* window)
{
    s_key_bindings[GLFW_KEY_W] = Key::Up;
    s_key_bindings[GLFW_KEY_D] = Key::Right;
    s_key_bindings[GLFW_KEY_S] = Key::Down;
    s_key_bindings[GLFW_KEY_A] = Key::Left;
    s_key_bindings[GLFW_KEY_ESCAPE] = Key::Escape;
    s_key_bindings[GLFW_KEY_SPACE] = Key::Space;
    s_key_bindings[GLFW_KEY_UP] = Key::UpArrow;
    s_key_bindings[GLFW_KEY_RIGHT] = Key::RightArrow;
    s_key_bindings[GLFW_KEY_DOWN] = Key::DownArrow;
    s_key_bindings[GLFW_KEY_LEFT] = Key::LeftArrow;
    s_key_bindings[GLFW_KEY_R] = Key::R;

    s_keys_downed.fill(false);
    s_keys_pressed.fill(false);

    s_window = window;
    glfwSetKeyCallback(window, KeyFun);
}

bool Inputs::IsKeyDowned(Key key)
{
    return s_keys_downed[static_cast<uint32_t>(key)];
}

bool Inputs::IsKeyPressed(Key key)
{
    bool ret = s_keys_pressed[static_cast<uint32_t>(key)];
    s_keys_pressed[static_cast<uint32_t>(key)] = false;
    return ret;
}

bool Inputs::IsMousePressed()
{
    int state = glfwGetMouseButton(s_window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        s_mouse_pressed = !s_mouse_downed;
        s_mouse_downed = true;
    }
    else if (state == GLFW_RELEASE)
    {
        s_mouse_downed = false;
    }

    return s_mouse_pressed;
}

bool Inputs::IsMouseDowned()
{
    int state = glfwGetMouseButton(s_window, GLFW_MOUSE_BUTTON_LEFT);
    return state == GLFW_RELEASE;
}

glm::dvec2 Inputs::GetMouseCoord()
{
    double xpos, ypos;
    glfwGetCursorPos(s_window, &xpos, &ypos);
    return { xpos/Engine::GetWidth()*2.f - 1.f , - (ypos/ Engine::GetHeight()*2.f - 1.f) };
}
