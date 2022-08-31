#pragma once
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#include <platinum/window/input.hpp>
#include <vector>

namespace plt
{

using CallbackID = uint64_t;
class Window : public NoCopy
{
public:
    Window() = default;
    // move: true
    Window(Window && window){
        std::swap(this->_implementation_id, window._implementation_id);
    };
    Window &operator=(Window && window){
        std::swap(this->_implementation_id, window._implementation_id);
        return *this;
    };

    static Result<> engine_init(void);

    static void engine_cleanup(void);

    static Result<Window> create(const char *title, int width, int height);

    static void destroy(Window &window);

    Result<> rename(const char *title);

    Result<> resize(int width, int height);

    Result<int> width();

    Result<int> height();

    void update();

    bool should_close();

    // window who will manage every callback, by default the first created window is the root window
    // this avoid architectural problems when multiple window can receive input but the backend can't differentiate them
    // and the mapping of a window to a callback may introduce slowdown for every input.
    void set_root();

    // note: on the glfw backend, the root window will be passed as a parameter to the callback function
    // I don't think one day we will need to use multi-window.
    Result<CallbackID> add_callback(WindowCallbackType callback_type, std::function<void(Window &, WindowCallback)> callback);

    Result<> remove_callback(CallbackID callback_id);

    static std::vector<const char *> required_extensions();

    void* handle() const { return _implementation_id;};
private:
    Window(void* id) : _implementation_id(id) {}
    void *_implementation_id = nullptr;
};

} // namespace plt
