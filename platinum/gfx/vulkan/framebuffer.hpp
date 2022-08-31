#pragma once
#include <platinum/gfx/vulkan/pass.hpp>
#include <platinum/gfx/vulkan/shader.hpp>
#include <platinum/gfx/vulkan/swapchain.hpp>
#include <vulkan/vulkan.h>

#include <platinum/gfx/vulkan/logical_device.hpp>

namespace plt
{

using VkFramebuffers = std::vector<VkFramebuffer>;
class Framebuffers
{
public:
    Framebuffers(VkFramebuffers &fb, Device &device, Swapchain &swapchain, RenderPass &render_pass)
        : _framebuffers(fb), _device(device), _swapchain(swapchain), _pass(render_pass)
    {
    }
    static Result<std::unique_ptr<Framebuffers>> create(Device &device, Swapchain &swapchain, RenderPass &render_pass);
    void destroy();
    const VkFramebuffers &handles() const { return _framebuffers; }

private:
    VkFramebuffers _framebuffers;

    Device &_device;
    Swapchain &_swapchain;
    RenderPass &_pass;
};

} // namespace plt
