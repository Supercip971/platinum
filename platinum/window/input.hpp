#pragma once

#include <functional>
#include <stdint.h>
namespace plt
{

enum WindowCallbackType : uint64_t
{
    WINDOW_CALLBACK_NONE = (0),
    WINDOW_CALLBACK_RESIZE = (1 << 0),
    WINDOW_CALLBACK_KEY_PRESS = (1 << 1),
    WINDOW_CALLBACK_MOUSE_MOVE = (1 << 2),
};

struct WindowCallback
{
    WindowCallbackType type; // only 1 bit in this bitfield can be set in this structure
    union
    {
        struct
        {
            int key;
            int scancode;
            int action;
            int modifier;
        } keyboard_input;

        struct
        {
            int width;
            int height;
        } window_resize;

        struct
        {
            int x;
            int y;
        } window_mouse_move;
    };
};
} // namespace plt
