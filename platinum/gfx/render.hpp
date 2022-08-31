#pragma once

#include <platinum/utility/result.hpp>
#include <platinum/window/window.hpp>
namespace plt
{
struct gfx
{
    virtual Result<> init(Window &target_surface) = 0;

    virtual Result<> draw_frame() = 0;
    virtual void deinit() = 0;
};

} // namespace plt
