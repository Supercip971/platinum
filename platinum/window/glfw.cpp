#include <GLFW/glfw3.h>

#include "window.hpp"
namespace plt
{

// this is only for accessing handle and input information. You should avoid as much as possible to use this directly
// I try to avoid singleton structure, but this is only for the backend so I don't think it is as much of a problem.

std::optional<Window> _root;

CallbackID _next_callback_id = 1;
struct Callbacks
{
    std::function<void(Window &, WindowCallback)> function;
    WindowCallbackType callback_type;
    CallbackID uid;
};

std::vector<Callbacks> _callbacks; // maybe separate later callback type

void _mouse_move_callback(GLFWwindow *window, double x, double y)
{
    (void)window;

    WindowCallback callback_arg = {
        .type = WINDOW_CALLBACK_MOUSE_MOVE,
        .window_mouse_move = {
            .x = int(x),
            .y = int(y),
        },
    };

    for (auto &callback : _callbacks)
    {
        if (callback.callback_type & WINDOW_CALLBACK_MOUSE_MOVE)
        {
            callback.function(*_root, callback_arg);
        }
    }
}

void _key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)window;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        debug$("escape pressed");
        glfwSetWindowShouldClose(window, true);
    }
    WindowCallback callback_arg = {
        .type = WINDOW_CALLBACK_KEY_PRESS,
        .keyboard_input = {
            .key = key,
            .scancode = scancode,
            .action = action,
            .modifier = mods,

        },
    };

    for (auto &callback : _callbacks)
    {
        if (callback.callback_type & WINDOW_CALLBACK_KEY_PRESS)
        {
            callback.function(*_root, callback_arg);
        }
    }
}

void _window_resize_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    WindowCallback callback_arg = {
        .type = WINDOW_CALLBACK_RESIZE,
        .window_resize = {
            .width = width,
            .height = height,
        },
    };

    for (auto &callback : _callbacks)
    {
        if (callback.callback_type & WINDOW_CALLBACK_RESIZE)
        {
            callback.function(*_root, callback_arg);
        }
    }
}

void _window_error_callback(int error, const char *description)
{
    error$("glfw: {}: {}", error, description);
}

GLFWwindow *window_id(Window &window)
{
    return static_cast<GLFWwindow *>(window.handle());
}

Result<> Window::engine_init(void)
{
    if (!glfwInit())
    {
        return Result<>::err("glfwInit failed");
    }

    glfwSetErrorCallback(_window_error_callback);
    _callbacks = {};
    return Result<>::ok({});
}

void Window::engine_cleanup(void)
{
    _callbacks.clear();
    glfwTerminate();
}

Result<Window> Window::create(const char *title, int width, int height)
{

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        return Result<Window>::err("glfwCreateWindow failed");
    }

    Window w((void *)window);
    if (!_root.has_value())
    {
        w.set_root();
    }

    return Result<Window>::ok(std::move(w));
}

void Window::destroy(Window &window)
{

    glfwDestroyWindow(window_id(window));
    if (_root.has_value() && _root.value()._implementation_id == window._implementation_id)
    {
        _root.reset();
        glfwTerminate();
    }
}

Result<> Window::rename(const char *title)
{
    glfwSetWindowTitle(window_id(*this), title);
    return Result<>::ok({});
}

Result<> Window::resize(int width, int height)
{
    glfwSetWindowSize(window_id(*this), width, height);
    return Result<>::ok({});
}

Result<int> Window::width()
{
    int _width = 0;
    int _height = 0;
    glfwGetWindowSize(window_id(*this), &_width, &_height);
    return Result<int>::ok(_width);
}

Result<int> Window::height()
{
    int _width = 0;
    int _height = 0;
    glfwGetWindowSize(window_id(*this), &_width, &_height);
    return Result<int>::ok(_height);
}

void Window::update()
{
    glfwPollEvents();
}

bool Window::should_close()
{
    return glfwWindowShouldClose(window_id(*this)) == GLFW_TRUE;
}

void Window::set_root()
{

    // this is cursed AF but this is the only way to callback
    // I'll make something better to avoid copying the window object (maybe do a shared pointer ?)
    _root = Window(this->_implementation_id);

    glfwSetWindowSizeCallback(window_id(*this), _window_resize_callback);
    glfwSetKeyCallback(window_id(*this), _key_callback);
    glfwSetCursorPosCallback(window_id(*this), _mouse_move_callback);
}

Result<CallbackID> Window::add_callback(WindowCallbackType callback_type, std::function<void(Window &, WindowCallback)> callback)
{
    Callbacks cb{
        .function = callback,
        .callback_type = callback_type,
        .uid = _next_callback_id,
    };
    _next_callback_id++;
    _callbacks.push_back(cb);
    return Result<CallbackID>::ok(cb.uid);
}

Result<> Window::remove_callback(CallbackID callback_id)
{
    for (auto it = _callbacks.begin(); it != _callbacks.end(); it++)
    {
        if (it->uid == callback_id)
        {
            _callbacks.erase(it);
            return Result<>::ok({});
        }
    }
    return Result<>::err("callback not found");
}

std::vector<const char *> Window::required_extensions()
{
    uint32_t ext_count = 0;
    std::vector<const char *> ext;

    const char **buff = glfwGetRequiredInstanceExtensions(&ext_count);

    for (uint32_t i = 0; i < ext_count; i++)
    {
        ext.push_back(buff[i]);
    }
    return ext;
}
} // namespace plt
