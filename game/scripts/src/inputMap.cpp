#include "inputMap.h"
#include <raylib.h>
bool input::isControllerMoving(){
    Vector2 mouseDelta = GetMouseDelta();
    Vector2 GamepadAxis = { GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X), GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) };

    if (GamepadAxis.x > -input::deadzone && GamepadAxis.x < input::deadzone) GamepadAxis.x = 0.0f;
    if (GamepadAxis.y > -input::deadzone && GamepadAxis.y < input::deadzone) GamepadAxis.y = 0.0f;

    return (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f || GamepadAxis.x != 0.0f || GamepadAxis.y != 0.0f);
}

bool input::isQuitting(){
    return WindowShouldClose() || IsKeyPressed(KEY_ESCAPE);
}
bool input::isLClickPressed(){
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
bool input::isRClickPressed(){
    return IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
}