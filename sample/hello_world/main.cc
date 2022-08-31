#include <fmt/core.h>
#include <iostream>
#include <platinum/gfx/render.hpp>
#include <platinum/gfx/vulkan/vulkan.hpp>
#include <platinum/log/log.hpp>
#include <platinum/utility/result.hpp>
#include <platinum/window/window.hpp>
plt::Result<int> test(int v)
{
    if (v == 2)
    {
        return (v);
    }

    return plt::Result<int>::err("test failed");
}

plt::Result<char *> test2(int c)
{
    try$(test(c));

    return (char *)"test2";
}

int main(int argc, char **argv)
{
    plt::Window::engine_init().assert();

    auto window = (plt::Window::create("hello world", 1920, 1080).unwrap());
    window.add_callback(plt::WINDOW_CALLBACK_KEY_PRESS,
                        [&](plt::Window &window, plt::WindowCallback callback)
                        {
                            (void)window;
                            fmt::print("key pressed: {}\n", callback.keyboard_input.key); })
        .assert();

    plt::gfx *gfx = new plt::VkGfx();

    gfx->init(window).assert();
    while (!window.should_close())
    {

        window.update();
        gfx->draw_frame().assert();
    }
    debug$("exiting");
    gfx->deinit();
    plt::Window::destroy(window);
    plt::Window::engine_cleanup();
    return 0;
}
