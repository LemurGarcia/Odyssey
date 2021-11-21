
#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

class Inputs
{
public:
    enum class Key
    {
        // Keyboard keys
        Up, Right, Down, Left, Escape, Space,
        UpArrow, RightArrow, DownArrow, LeftArrow,
        R,

        // Gamepad keys
        GamePad,
        GP_A, GP_B, GP_X, GP_Y, GP_LeftBumper, GP_RightBumper, GP_Back, GP_Start,
        GP_Guide, GP_LeftThumb, GP_RightThumb, GP_Up, GP_Right, GP_Down, GP_Left,

        GP_AxisLeft, GP_AxisRight, GP_Trigger,

        Count,
    };

    static void Initialize(struct GLFWwindow* window);
    static bool IsKeyDowned(Key key);
    static bool IsKeyPressed(Key key);
    static bool IsMouseDowned();
    static bool IsMousePressed();
    static glm::dvec2 GetMouseCoord();
};

