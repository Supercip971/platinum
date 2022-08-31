#pragma once
#include <platinum/utility/result.hpp>
#include <platinum/utility/trait.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

namespace plt
{
class Surface : public NoCopy
{
public:
    Surface(){};
    Surface(VkSurfaceKHR surface, VkExtent2D size) : _surface(surface), _size(size){};
    Surface(Surface &&other)
    {
        std::swap(_surface, other._surface);
    }
    Surface &operator=(Surface &&other)
    {
        std::swap(_surface, other._surface);
        return *this;
    }

    constexpr VkExtent2D size() const { return _size; }

    constexpr void resize(VkExtent2D size) { _size = size; }

    VkSurfaceKHR &handle() { return _surface; };
    const VkSurfaceKHR &handle() const { return _surface; };

private:
    VkSurfaceKHR _surface;
    VkExtent2D _size;
    friend class Instance;
};

} // namespace plt
